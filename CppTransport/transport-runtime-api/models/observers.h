//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __observers_H_
#define __observers_H_


#include <iostream>
#include <iomanip>
#include <sstream>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/manager/data_manager.h"
#include "transport-runtime-api/tasks/task_configurations.h"

#include <boost/timer/timer.hpp>


namespace transport
  {


    //! A stepping observer is the basic type of observer object.
    //! It is capable of keeping track of time steps and matching them
    //! to the list of steps which should be stored according to the
    //! active time-step storage policy
    class stepping_observer
      {

      public:

        //! Create a stepping observer object
        stepping_observer(const std::vector<time_config>& l)
          : time_step(0), storage_list(l)
          {
            current_config = storage_list.begin();
          }

        //! Get current time step
        unsigned int get_current_time_step() { return(this->time_step); }

        //! Advance time-step counter
        void step() { this->time_step++; }

        //! Advance list of steps which should be stored, according to the time-storage policy
        void advance_storage_list() { if(this->current_config != this->storage_list.end()) this->current_config++; }

        //! Query whether the current time step should be stored
        bool store_time_step() { return((this->current_config != this->storage_list.end()) && (this->time_step == (*(this->current_config)).serial)); }

      private:

        //! Records current time step
        unsigned int time_step;

        //! List of steps which should be stored
        const std::vector<time_config>& storage_list;

        //! Pointer to current location in this of stored time steps
        std::vector<time_config>::const_iterator current_config;

      };


    //! A timing observer is a more sophisticated type of observer; it keeps track
    //! of how long is spent during the integration (and the batching process)
    class timing_observer: public stepping_observer
      {

      public:

        //! Create a timing observer object
        timing_observer(const std::vector<time_config>& l, double t_int=1.0, bool s=false, unsigned int p=3)
          : stepping_observer(l), t_interval(t_int), silent(s), first_step(true), t_last(0), precision(p)
          {
            batching_timer.stop();
            // leave the integration timer running, so it also records start-up time associated with the integration,
            // eg. setting up initial conditions or copying data to an offload device such as a GPU
          }

        //! Prepare for a batching step
        template <typename Level>
        void start_batching(double t, boost::log::sources::severity_logger<Level>& logger, Level lev)
          {
            std::string rval = "";

            this->integration_timer.stop();
            this->batching_timer.start();

            // should we emit output?
            // only do so if: not silent, and: either, first step, or enough time has elapsed
            if(!this->silent && (this->first_step || t > this->t_last + this->t_interval))
              {
                this->t_last = t;

                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OBSERVER_TIME << " = " << std::scientific << std::setprecision(this->precision) << t;
                if(first_step) msg << " " << __CPP_TRANSPORT_OBSERVER_ELAPSED << " =" << this->integration_timer.format();

                BOOST_LOG_SEV(logger, lev) << msg.str();

                first_step = false;
              }
          }

        //! Conclude a batching step
        void stop_batching()
          {
            this->batching_timer.stop();
            this->integration_timer.start();
          }

        //! Stop the running timers - should only be called at the end of an integration
        void stop_timers()
          {
            this->batching_timer.stop();
            this->integration_timer.stop();
          }

        //! Get the total elapsed integration time
        boost::timer::nanosecond_type get_integration_time() const { return(this->integration_timer.elapsed().wall); }

        //! Get the total elapsed batching time
        boost::timer::nanosecond_type get_batching_time() const { return(this->batching_timer.elapsed().wall); }


        // INTERNAL DATA

      private:

        //! Do we generate output during observations?
        bool                    silent;

        //! Is this the first batching step? Used to decide whether to issue output
        bool                    first_step;

        //! Last time at which output was emitted;
        //! used to decide whether to emit output during
        //! the next observation
        double                  t_last;

        //! Time interval at which to issue updates
        double                  t_interval;

        //! Numerical precision to be used when issuing updates
        unsigned int            precision;

        //! Timer for the integration
        boost::timer::cpu_timer integration_timer;

        //! Timer for batching
        boost::timer::cpu_timer batching_timer;
      };


    // Observer: records results from a single twopf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class twopf_singleconfig_batch_observer: public timing_observer
      {

      public:

        twopf_singleconfig_batch_observer(typename data_manager<number>::twopf_batcher& b, const twopf_kconfig& c,
                                          const std::vector<time_config>& l,
                                          unsigned int bg_sz, unsigned int tw_sz,
                                          unsigned int bg_st, unsigned int tw_st,
                                          double t_int=1.0, bool s=true, unsigned int p=3)
          : timing_observer(l, t_int, s, p), batcher(b), k_config(c),
            backg_size(bg_sz), twopf_size(tw_sz),
            backg_start(bg_st), twopf_start(tw_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->store_time_step())
              {
                if(this->k_config.store_background)
                  {
                    std::vector<number> bg_x(this->backg_size);

                    for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[this->backg_start + i];
                    this->batcher.push_backg(this->get_current_time_step(), bg_x);
                  }

                std::vector<number> tpf_x(this->twopf_size);

                for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_start + i];
                this->batcher.push_twopf(this->get_current_time_step(), this->k_config.serial, tpf_x);

                this->advance_storage_list();
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

      private:

        const twopf_kconfig&                          k_config;
        typename data_manager<number>::twopf_batcher& batcher;

        unsigned int                                  backg_size;
        unsigned int                                  twopf_size;

        unsigned int                                  backg_start;
        unsigned int                                  twopf_start;
      };


    // Observer: records results from a single threepf k-configuration
    // this is suitable for an OpenMP or MPI type integrator

    template <typename number>
    class threepf_singleconfig_batch_observer: public timing_observer
      {

      public:

        threepf_singleconfig_batch_observer(typename data_manager<number>::threepf_batcher& b, const threepf_kconfig& c,
                                            const std::vector<time_config>& l,
                                            unsigned int bg_sz, unsigned int tw_sz, unsigned int th_sz,
                                            unsigned int bg_st,
                                            unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                            unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                            unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                            unsigned int th_st,
                                            double t_int=1.0, bool s=true, unsigned int p=3)
          : timing_observer(l, t_int, s, p), batcher(b), k_config(c),
            backg_size(bg_sz), twopf_size(tw_sz), threepf_size(th_sz),
            backg_start(bg_st),
            twopf_re_k1_start(tw_re_k1_st), twopf_im_k1_start(tw_im_k1_st),
            twopf_re_k2_start(tw_re_k2_st), twopf_im_k2_start(tw_im_k2_st),
            twopf_re_k3_start(tw_re_k3_st), twopf_im_k3_start(tw_im_k3_st),
            threepf_start(th_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->store_time_step())
              {
                if(this->k_config.store_background)
                  {
                    std::vector<number> bg_x(this->backg_size);

                    for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[this->backg_start + i];
                    this->batcher.push_backg(this->get_current_time_step(), bg_x);
                  }

                if(this->k_config.store_twopf_k1)
                  {
                    std::vector<number> tpf_x(this->twopf_size);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_re_k1_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[0], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_im_k1_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[0], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                  }

                if(this->k_config.store_twopf_k2)
                  {
                    std::vector<number> tpf_x(this->twopf_size);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_re_k2_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[1], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_im_k2_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[1], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                  }

                if(this->k_config.store_twopf_k3)
                  {
                    std::vector<number> tpf_x(this->twopf_size);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_re_k3_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[2], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[this->twopf_im_k3_start + i];
                    this->batcher.push_twopf(this->get_current_time_step(), this->k_config.index[2], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                  }

                std::vector<number> thpf_x(this->threepf_size);
                for(unsigned int i = 0; i < this->threepf_size; i++) thpf_x[i] = x[this->threepf_start + i];
                this->batcher.push_threepf(this->get_current_time_step(), this->k_config.serial, thpf_x);

                this->advance_storage_list();
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

      private:

        const threepf_kconfig&                          k_config;
        typename data_manager<number>::threepf_batcher& batcher;

        unsigned int                                    backg_size;
        unsigned int                                    twopf_size;
        unsigned int                                    threepf_size;

        unsigned int                                    backg_start;
        unsigned int                                    twopf_re_k1_start;
        unsigned int                                    twopf_im_k1_start;
        unsigned int                                    twopf_re_k2_start;
        unsigned int                                    twopf_im_k2_start;
        unsigned int                                    twopf_re_k3_start;
        unsigned int                                    twopf_im_k3_start;
        unsigned int                                    threepf_start;
      };


    // Observer: records results from a batch of twopf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class twopf_groupconfig_batch_observer: public timing_observer
      {

      public:

        twopf_groupconfig_batch_observer(typename data_manager<number>::twopf_batcher& b,
                                         const work_queue<twopf_kconfig>::device_work_list& c,
                                         const std::vector<time_config>& l,
                                         unsigned int bg_sz, unsigned int tw_sz,
                                         unsigned int bg_st, unsigned int tw_st,
                                         double t_int=1.0, bool s=false, unsigned int p=3)
          : timing_observer(l, t_int, s, p), batcher(b), work_list(c),
            backg_size(bg_sz), twopf_size(tw_sz),
            backg_start(bg_st), twopf_start(tw_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->store_time_step())
              {
                unsigned int n = work_list.size();

                // loop through all k-configurations
                for(unsigned int c = 0; c < n; c++)
                  {
                    if(this->work_list[c].store_background)
                      {
                        std::vector<number> bg_x(this->backg_size);

                        for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[(this->backg_start + i)*n + c];
                        this->batcher.push_backg(this->get_current_time_step(), bg_x);
                      }

                    std::vector<number> tpf_x(this->twopf_size);

                    for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_start + i)*n + c];
                    this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].serial, tpf_x);
                  }

                this->advance_storage_list();
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }

      private:

        const work_queue<twopf_kconfig>::device_work_list& work_list;
        typename data_manager<number>::twopf_batcher&      batcher;

        unsigned int                                       backg_size;
        unsigned int                                       twopf_size;

        unsigned int                                       backg_start;
        unsigned int                                       twopf_start;
      };


    // Observer: records results from a batch of threepf k-configurations
    // this is suitable for a GPU type integrator

    template <typename number>
    class threepf_groupconfig_batch_observer: public timing_observer
      {

      public:

        threepf_groupconfig_batch_observer(typename data_manager<number>::threepf_batcher& b,
                                           const work_queue<threepf_kconfig>::device_work_list& c,
                                           const std::vector<time_config>& l,
                                           unsigned int bg_sz, unsigned int tw_sz, unsigned int th_sz,
                                           unsigned int bg_st,
                                           unsigned int tw_re_k1_st, unsigned int tw_im_k1_st,
                                           unsigned int tw_re_k2_st, unsigned int tw_im_k2_st,
                                           unsigned int tw_re_k3_st, unsigned int tw_im_k3_st,
                                           unsigned int th_st,
                                           double t_int=1.0, bool s=false, unsigned int p=3)
          : timing_observer(l, t_int, s, p), batcher(b), work_list(c),
            backg_size(bg_sz), twopf_size(tw_sz), threepf_size(th_sz),
            backg_start(bg_st),
            twopf_re_k1_start(tw_re_k1_st), twopf_im_k1_start(tw_im_k1_st),
            twopf_re_k2_start(tw_re_k2_st), twopf_im_k2_start(tw_im_k2_st),
            twopf_re_k3_start(tw_re_k3_st), twopf_im_k3_start(tw_im_k3_st),
            threepf_start(th_st)
          {
          }

        //! Push the current state to the batcher
        template <typename State>
        void push(const State& x)
          {
            if(this->store_time_step())
              {
                unsigned int n = this->work_list.size();

                // loop through all k-configurations
                for(unsigned int c = 0; c < n; c++)
                  {
                    if(this->work_list[c].store_background)
                      {
                        std::vector<number> bg_x(this->backg_size);

                        for(unsigned int i = 0; i < this->backg_size; i++) bg_x[i] = x[(this->backg_start + i)*n + c];
                        this->batcher.push_backg(this->get_current_time_step(), bg_x);
                      }

                    if(this->work_list[c].store_twopf_k1)
                      {
                        std::vector<number> tpf_x(this->twopf_size);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_re_k1_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[0], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_im_k1_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[0], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                      }

                    if(this->work_list[c].store_twopf_k2)
                      {
                        std::vector<number> tpf_x(this->twopf_size);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_re_k2_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[1], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_im_k2_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[1], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                      }

                    if(this->work_list[c].store_twopf_k3)
                      {
                        std::vector<number> tpf_x(this->twopf_size);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_re_k3_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[2], tpf_x, data_manager<number>::threepf_batcher::real_twopf);

                        for(unsigned int i = 0; i < this->twopf_size; i++) tpf_x[i] = x[(this->twopf_im_k3_start + i)*n + c];
                        this->batcher.push_twopf(this->get_current_time_step(), this->work_list[c].index[2], tpf_x, data_manager<number>::threepf_batcher::imag_twopf);
                      }

                    std::vector<number> thpf_x(this->threepf_size);
                    for(unsigned int i = 0; i < this->threepf_size; i++) thpf_x[i] = x[(this->threepf_start + i)*n + c];
                    this->batcher.push_threepf(this->get_current_time_step(), this->work_list[c].serial, thpf_x);
                  }

                this->advance_storage_list();
              }

            this->step();
          }

        //! Return logger
        boost::log::sources::severity_logger<typename data_manager<number>::log_severity_level>& get_log() { return(this->batcher.get_log()); }

        //! Return number of k-configurations in this group
        unsigned int group_size() const { return(this->work_list.size()); }

      private:

        const work_queue<threepf_kconfig>::device_work_list& work_list;
        typename data_manager<number>::threepf_batcher&      batcher;

        unsigned int                                         backg_size;
        unsigned int                                         twopf_size;
        unsigned int                                         threepf_size;

        unsigned int                                         backg_start;
        unsigned int                                         twopf_re_k1_start;
        unsigned int                                         twopf_im_k1_start;
        unsigned int                                         twopf_re_k2_start;
        unsigned int                                         twopf_im_k2_start;
        unsigned int                                         twopf_re_k3_start;
        unsigned int                                         twopf_im_k3_start;
        unsigned int                                         threepf_start;

      };

  } // namespace transport


#endif //__observers_H_
