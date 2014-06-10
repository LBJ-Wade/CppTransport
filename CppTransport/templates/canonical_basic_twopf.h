// backend=cpp minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// MPI implementation

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include "transport-runtime-api/transport.h"

#include "$$__CORE"


namespace transport
  {
    // set up a state type for 2pf integration
    template <typename number>
    using twopf_state = std::vector<number>;

    // set up a state type for 3pf integration
    template <typename number>
    using threepf_state = std::vector<number>;

    namespace $$__MODEL_pool
      {
        const static std::string backend = "MPI";
      }


    // *********************************************************************************************


    // CLASS FOR $$__MODEL 'basic', ie., MPI implementation
    template <typename number>
    class $$__MODEL_basic : public $$__MODEL<number>
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

        $$__MODEL_basic(instance_manager<number>* mgr)
        : $$__MODEL<number>(mgr)
          {
          }

		    ~$$__MODEL_basic() = default;

        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_backend() const override { return($$__MODEL_pool::backend); }

        // BACKEND INTERFACE

      public:

        // Set up a context
        virtual context backend_get_context() override;

        // Integrate background and 2-point function on the CPU
        virtual void backend_process_twopf(work_queue<twopf_kconfig>& work, const integration_task<number>* tk,
                                           typename data_manager<number>::twopf_batcher& batcher,
                                           bool silent = false) override;

        // Integrate background, 2-point function and 3-point function on the CPU
        virtual void backend_process_threepf(work_queue<threepf_kconfig>& work, const integration_task<number>* tk,
                                             typename data_manager<number>::threepf_batcher& batcher,
                                             bool silent = false) override;

        virtual unsigned int backend_twopf_state_size(void)   const override { return($$__MODEL_pool::twopf_state_size); }
        virtual unsigned int backend_threepf_state_size(void) const override { return($$__MODEL_pool::threepf_state_size); }

        virtual bool supports_per_configuration_statistics(void) const override { return(true); }


        // INTERNAL API

      protected:

        void twopf_kmode(const twopf_kconfig& kconfig, const integration_task<number>* tk,
                         typename data_manager<number>::twopf_batcher& batcher,
                         boost::timer::nanosecond_type& int_time, boost::timer::nanosecond_type& batch_time);

        void threepf_kmode(const threepf_kconfig&, const integration_task<number>* tk,
                           typename data_manager<number>::threepf_batcher& batcher,
                           boost::timer::nanosecond_type& int_time, boost::timer::nanosecond_type& batch_time);

        void populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                               const parameters<number>& p, const std::vector<number>& ic, bool imaginary = false);

        void populate_threepf_ic(threepf_state<number>& x, unsigned int start, const threepf_kconfig& kconfig,
                                 double Ninit, const parameters<number>& p, const std::vector<number>& ic);

      };


    // integration - 2pf functor
    template <typename number>
    class $$__MODEL_basic_twopf_functor: public constexpr_flattener<$$__NUMBER_FIELDS>
      {

      public:

        $$__MODEL_basic_twopf_functor(const parameters<number>& p, double k)
          : params(p), k_mode(k)
          {
          }

        void operator ()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t);

      private:

        const parameters<number>& params;

        const double k_mode;

      };


    // integration - observer object for 2pf
    template <typename number>
    class $$__MODEL_basic_twopf_observer: public twopf_singleconfig_batch_observer<number>
      {

      public:

        $$__MODEL_basic_twopf_observer(typename data_manager<number>::twopf_batcher& b, const twopf_kconfig& c,
                                       const std::vector<time_config>& l)
          : twopf_singleconfig_batch_observer<number>(b, c, l,
                                                      $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size,
                                                      $$__MODEL_pool::backg_start, $$__MODEL_pool::twopf_start)
          {
          }

        void operator ()(const twopf_state<number>& x, double t);

      };


    // integration - 3pf functor
    template <typename number>
    class $$__MODEL_basic_threepf_functor: public constexpr_flattener<$$__NUMBER_FIELDS>
      {

      public:
        $$__MODEL_basic_threepf_functor(const parameters<number>& p, double k1, double k2, double k3)
          : params(p), kmode_1(k1), kmode_2(k2), kmode_3(k3)
          {
          }

        void operator ()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __dt);

      private:

        const parameters<number>& params;

        const double kmode_1;
        const double kmode_2;
        const double kmode_3;

      };


    // integration - observer object for 3pf
    template <typename number>
    class $$__MODEL_basic_threepf_observer: public threepf_singleconfig_batch_observer<number>
      {

      public:
        $$__MODEL_basic_threepf_observer(typename data_manager<number>::threepf_batcher& b, const threepf_kconfig& c,
                                         const std::vector<time_config>& l)
          : threepf_singleconfig_batch_observer<number>(b, c, l,
                                                        $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size, $$__MODEL_pool::threepf_size,
                                                        $$__MODEL_pool::backg_start,
                                                        $$__MODEL_pool::twopf_re_k1_start, $$__MODEL_pool::twopf_im_k1_start,
                                                        $$__MODEL_pool::twopf_re_k2_start, $$__MODEL_pool::twopf_im_k2_start,
                                                        $$__MODEL_pool::twopf_re_k3_start, $$__MODEL_pool::twopf_im_k3_start,
                                                        $$__MODEL_pool::threepf_start)
          {
          }

        void operator ()(const threepf_state<number>& x, double t);

      };

    
    // BACKEND INTERFACE


    // generate a context
    template <typename number>
    context $$__MODEL_basic<number>::backend_get_context(void)
      {
        context ctx;

        // set up just one device
        ctx.add_device($$__MODEL_pool::backend);

        return(ctx);
      }

    // process work queue for twopf
    template <typename number>
    void $$__MODEL_basic<number>::backend_process_twopf(work_queue<twopf_kconfig>& work, const integration_task<number>* tk,
                                                        typename data_manager<number>::twopf_batcher& batcher,
                                                        bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
            << "** MPI compute backend processing twopf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << work_msg.str();
//        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<twopf_kconfig>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const work_queue<twopf_kconfig>::device_work_list list = queues[0];

        for(unsigned int i = 0; i < list.size(); i++)
          {
            boost::timer::nanosecond_type int_time;
            boost::timer::nanosecond_type batch_time;

            try
              {
                // write the time history for this k-configuration
                this->twopf_kmode(list[i], tk, batcher, int_time, batch_time);

                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
                    << __CPP_TRANSPORT_SOLVING_CONFIG << " " << list[i].serial << " (" << i+1
                    << " " __CPP_TRANSPORT_OF << " " << list.size() << "), "
                    << __CPP_TRANSPORT_INTEGRATION_TIME << " = " << format_time(int_time) << " | "
                    << __CPP_TRANSPORT_BATCHING_TIME << " = " << format_time(batch_time);
               }
            catch(std::overflow_error& xe)
              {
                batcher.report_integration_failure();

                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::error)
                    << "!! " __CPP_TRANSPORT_FAILED_CONFIG << " " << list[i].serial << " (" << i+1
                    << " " __CPP_TRANSPORT_OF << " " << list.size() << ") | " << list[i];
              }
          }
      }


    template <typename number>
    void $$__MODEL_basic<number>::twopf_kmode(const twopf_kconfig& kconfig, const integration_task<number>* tk,
                                              typename data_manager<number>::twopf_batcher& batcher,
                                              boost::timer::nanosecond_type& int_time, boost::timer::nanosecond_type& batch_time)
      {
        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $$__MODEL_basic_twopf_observer<number> obs(batcher, kconfig, tk->get_time_config_list());

        // set up a functor to evolve this system
        $$__MODEL_basic_twopf_functor<number> rhs(tk->get_params(), kconfig.k);

        // set up a state vector
        twopf_state<number> x;
        x.resize($$__MODEL_pool::twopf_state_size);

        // fix initial conditions - background
        const std::vector<number>& ics = tk->get_ics_vector();
        x[$$__MODEL_pool::backg_start + FLATTEN($$__A)] = $$// ics[$$__A];

        // fix initial conditions - 2pf
        const std::vector<double>& times = tk->get_integration_step_times();
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_start, kconfig.k, times.front(), tk->get_params(), ics);

        using namespace boost::numeric::odeint;
        integrate_times($$__MAKE_PERT_STEPPER{twopf_state<number>}, rhs, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

        obs.stop_timers();
        int_time = obs.get_integration_time();
        batch_time = obs.get_batching_time();
      }


    // make initial conditions for each component of the 2pf
    // x         - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
    // start     - starting position of twopf components within the state vector
    // kmode     - *comoving normalized* wavenumber for which we will compute the twopf
    // Ninit     - initial time
    // p         - parameters
    // ics       - iniitial conditions for the background fields (or fields+momenta)
    // imaginary - whether to populate using real or imaginary components of the 2pf
    template <typename number>
    void $$__MODEL_basic<number>::populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                                                    const parameters<number>& p, const std::vector<number>& ics, bool imaginary)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::twopf_size);

        x[start + FLATTEN($$__A,$$__B)] = imaginary ? this->make_twopf_im_ic($$__A, $$__B, kmode, Ninit, p, ics) : this->make_twopf_re_ic($$__A, $$__B, kmode, Ninit, p, ics) $$// ;
      }


    // THREE-POINT FUNCTION INTEGRATION


    template <typename number>
    void $$__MODEL_basic<number>::backend_process_threepf(work_queue<threepf_kconfig>& work, const integration_task<number>* tk,
                                                          typename data_manager<number>::threepf_batcher& batcher,
                                                          bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
          << "** MPI compute backend processing threepf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << work_msg.str();
//        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be only one device with this backend)
        assert(work.size() == 1);
        const work_queue<threepf_kconfig>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const work_queue<threepf_kconfig>::device_work_list list = queues[0];

        // step through the queue, solving for the three-point functions in each case
        for(unsigned int i = 0; i < list.size(); i++)
          {
            boost::timer::nanosecond_type int_time;
            boost::timer::nanosecond_type batch_time;

            try
              {
                // write the time history for this k-configuration
                this->threepf_kmode(list[i], tk, batcher, int_time, batch_time);

                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
                    << "** " << __CPP_TRANSPORT_SOLVING_CONFIG << " " << list[i].serial << " (" << i+1
                    << " " __CPP_TRANSPORT_OF << " " << list.size() << "), "
                    << __CPP_TRANSPORT_INTEGRATION_TIME << " = " << format_time(int_time) << " | "
                    << __CPP_TRANSPORT_BATCHING_TIME << " = " << format_time(batch_time);
              }
            catch(std::overflow_error& xe)
              {
                batcher.report_integration_failure();

                BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
                    << "!! " __CPP_TRANSPORT_FAILED_CONFIG << " " << list[i].serial << " (" << i+1
                    << " " __CPP_TRANSPORT_OF << " " << list.size() << ") | " << list[i];
              }
          }
      }


    template <typename number>
    void $$__MODEL_basic<number>::threepf_kmode(const threepf_kconfig& kconfig, const integration_task<number>* tk,
                                                typename data_manager<number>::threepf_batcher& batcher,
                                                boost::timer::nanosecond_type& int_time, boost::timer::nanosecond_type& batch_time)
      {
        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $$__MODEL_basic_threepf_observer<number> obs(batcher, kconfig, tk->get_time_config_list());

        // set up a functor to evolve this system
        $$__MODEL_basic_threepf_functor<number>  rhs(tk->get_params(), kconfig.k1, kconfig.k2, kconfig.k3);

        // set up a state vector
        threepf_state<number> x;
        x.resize($$__MODEL_pool::threepf_state_size);

        // fix initial conditions - background (don't need explicit FLATTEN since it would appear on both sides)
        const std::vector<number>& ics = tk->get_ics_vector();
        x[$$__MODEL_pool::backg_start + $$__A] = $$// ics[$$__A];

        // fix initial conditions - real 2pfs
        const std::vector<double>& times = tk->get_integration_step_times();
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k1_start, kconfig.k1, times.front(), tk->get_params(), ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k2_start, kconfig.k2, times.front(), tk->get_params(), ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k3_start, kconfig.k3, times.front(), tk->get_params(), ics, false);

        // fix initial conditions - imaginary 2pfs
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k1_start, kconfig.k1, times.front(), tk->get_params(), ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k2_start, kconfig.k2, times.front(), tk->get_params(), ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k3_start, kconfig.k3, times.front(), tk->get_params(), ics, true);

        // fix initial conditions - threepf
        this->populate_threepf_ic(x, $$__MODEL_pool::threepf_start, kconfig, times.front(), tk->get_params(), ics);

        using namespace boost::numeric::odeint;
        integrate_times( $$__MAKE_PERT_STEPPER{threepf_state<number>}, rhs, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

        obs.stop_timers();
        int_time = obs.get_integration_time();
        batch_time = obs.get_batching_time();
      }


    template <typename number>
    void $$__MODEL_basic<number>::populate_threepf_ic(threepf_state<number>& x, unsigned int start,
                                                      const threepf_kconfig& kconfig, double Ninit,
                                                      const parameters<number>& p, const std::vector<number>& ics)
      {
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_twopf_functor<number>::operator()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t)
      {
        const auto $$__PARAMETER[1]  = this->params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __x[FLATTEN($$__A)];
        const auto __Mp              = this->params.get_Mp();
        const auto __k               = this->k_mode;
        const auto __a               = exp(__t);
        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;

        const auto __tpf_$$__A_$$__B = $$// __x[$$__MODEL_pool::twopf_start + FLATTEN($$__A,$$__B)];

        $$__TEMP_POOL{"const auto $1 = $2;"}

#undef __background
#undef __dtwopf
#define __background(a)   __dxdt[$$__MODEL_pool::backg_start + FLATTEN(a)]
#define __dtwopf(a,b)     __dxdt[$$__MODEL_pool::twopf_start + FLATTEN(a,b)]

        // evolve the background
        __background($$__A) = $$__U1_PREDEF[A]{__Hsq, __eps};

        // set up components of the u2 tensor
        const auto __u2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

        // evolve the 2pf
        // here, we are dealing only with the real part - which is symmetric.
        // so the index placement is not important
        __dtwopf($$__A, $$__B) = 0 $$// + $$__SUM_COORDS[C] __u2_$$__A_$$__C*__tpf_$$__C_$$__B;
        __dtwopf($$__A, $$__B) += 0 $$// + $$__SUM_COORDS[C] __u2_$$__B_$$__C*__tpf_$$__A_$$__C;
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_twopf_observer<number>::operator()(const twopf_state<number>& x, double t)
      {
        this->start_batching(t, this->get_log(), data_manager<number>::normal);
        this->push(x);
        this->stop_batching();
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_threepf_functor<number>::operator()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __t)
      {
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_threepf_observer<number>::operator()(const threepf_state<number>& x, double t)
      {
        this->start_batching(t, this->get_log(), data_manager<number>::normal);
        this->push(x);
        this->stop_batching();
      }


    }   // namespace transport


#endif  // $$__GUARD