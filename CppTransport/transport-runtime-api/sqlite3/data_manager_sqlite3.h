//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_sqlite3_H_
#define __data_manager_sqlite3_H_


#include <sstream>
#include <list>
#include <string>
#include <vector>

#include "transport-runtime-api/manager/repository.h"
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/models/model.h"

#include "transport-runtime-api/utilities/formatter.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/sqlite3_operations.h"


#define __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM "worker"
#define __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN  ".sqlite"


namespace transport
  {

    namespace
      {

        const std::string default_excpt_a = __CPP_TRANSPORT_DATACTR_OPEN_A;
        const std::string default_excpt_b = __CPP_TRANSPORT_DATACTR_OPEN_B;

      }   // unnamed namespace


    // implements the data_manager interface using sqlite3
    template <typename number>
    class data_manager_sqlite3: public data_manager<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager_sqlite3 instance
        data_manager_sqlite3(unsigned int cp)
          : data_manager<number>(cp), temporary_container_serial(0)
          {
          }

        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // WRITER HANDLONG -- implements a 'data_manager' interface

      public:

        //! Create data files for a new integration_writer object, including the data container.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_writer(typename repository<number>::integration_writer& ctr) override;

        //! Close an open integration_writer object.

        //! Any open sqlite3 handles are closed, meaning that any integration_writer objects will be invalidated.
        //! After closing, attempting to use an integration_writer will lead to unsubtle errors.
        virtual void close_writer(typename repository<number>::integration_writer& ctr) override;

		    //! Create data files for a new derived_content_writer object.
		    virtual void create_writer(typename repository<number>::derived_content_writer& ctr) override;

		    //! Close an open derived_content_writer object.

		    //! Any open sqlite3 handles are closed. Attempting to use the writer after closing
		    //! will lead to unsubtle errors.
		    virtual void close_writer(typename repository<number>::derived_content_writer& ctr) override;


        // WRITE INDEX TABLES -- implements a 'data_manager' interface

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, twopf_task<number>* tk,
                                   unsigned int Nfields) override;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_writer& ctr, threepf_task<number>* tk,
                                   unsigned int Nfields) override;


        // TASK FILES -- implements a 'data_manager' interface

      public:

        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<twopf_kconfig>& queue) override;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<threepf_kconfig>& queue) override;

        //! Create a list of task assignments, over a number of devices, for a work queue of output_task_element-s
        virtual void create_taskfile(typename repository<number>::derived_content_writer& writer, const work_queue< output_task_element<number> >& queue) override;

        //! Read a list of task assignments for a particular worker
        virtual std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker) override;


        // TEMPORARY CONTAINERS  -- implements a 'data_manager' interface

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual typename data_manager<number>::twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir,
                                                                                         const boost::filesystem::path& logdir,
                                                                                         unsigned int worker, unsigned int Nfields,
                                                                                         typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                         boost::timer::cpu_timer& integration_timer) override;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual typename data_manager<number>::threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                             const boost::filesystem::path& logdir,
                                                                                             unsigned int worker, unsigned int Nfields,
                                                                                             typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                             boost::timer::cpu_timer& integration_timer) override;

        //! Aggregate a temporary twopf container into a principal container
        virtual void aggregate_twopf_batch(typename repository<number>::integration_writer& ctr,
                                           const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) override;

        //! Aggregate a temporary threepf container into a principal container
        virtual void aggregate_threepf_batch(typename repository<number>::integration_writer& ctr,
                                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk) override;


        // DATA PIPES -- implements a 'data_manager' interface

      public:

        //! Create a new datapipe
        virtual typename data_manager<number>::datapipe create_datapipe(const boost::filesystem::path& logdir,
                                                                        const boost::filesystem::path& tempdir,
                                                                        typename data_manager<number>::output_group_finder finder,
                                                                        typename data_manager<number>::derived_content_dispatch_function dispatcher,
                                                                        unsigned int worker, boost::timer::cpu_timer& timer) override;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_sample(typename data_manager<number>::datapipe* pipe,
                                      const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample, unsigned int worker) override;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        void pull_twopf_kconfig_sample(typename data_manager<number>::datapipe* pipe, const std::vector<unsigned int>& serial_numbers,
                                       typename std::vector< typename data_manager<number>::twopf_configuration >& sample, unsigned int worker) override;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        void pull_threepf_kconfig_sample(typename data_manager<number>::datapipe* pipe, const std::vector<unsigned int>& serial_numbers,
                                         typename std::vector<typename data_manager<number>::threepf_configuration>& sample, unsigned int worker) override;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(typename data_manager<number>::datapipe* pipe, unsigned int id,
                                                 const std::vector<unsigned int>& t_serials, std::vector<number>& sample,
                                                 unsigned int worker) override;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(typename data_manager<number>::datapipe* pipe, unsigned int id,
                                            const std::vector<unsigned int>& t_serials, unsigned int k_serial,
                                            std::vector<number>& sample, typename data_manager<number>::datapipe_twopf_type type,
                                            unsigned int worker) override;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(typename data_manager<number>::datapipe* pipe, unsigned int id,
                                              const std::vector<unsigned int>& t_serials,
                                              unsigned int kserial, std::vector<number>& sample, unsigned int worker) override;

      protected:

        //! Attach an output_group to a pipe
        typename repository<number>::template output_group<typename repository<number>::integration_payload>
          datapipe_attach(typename data_manager<number>::datapipe* pipe,
                          typename data_manager<number>::output_group_finder& finder,
                          integration_task<number>* tk, const std::list<std::string>& tags);

        //! Detach an output_group from a pipe
        void datapipe_detach(typename data_manager<number>::datapipe* pipe);


		    // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Replace a temporary twopf container with a new one
        void replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                          unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                          typename data_manager<number>::replacement_action action);

        //! Replace a temporary threepf container with a new one
        void replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                            unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                            typename data_manager<number>::replacement_action action);

        //! Generate the name for a temporary container
        boost::filesystem::path generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker);


        // INTERNAL DATA

      private:

        //! List of open sqlite3 connexions
        std::list< sqlite3* > open_containers;

        //! Serial number used to distinguish
        //! the set of temporary containers associated with this worker.
		    //! Begins at zero and is incremented as temporary containers are generated.
        unsigned int          temporary_container_serial;

      };


    // CONSTRUCTOR, DESTRUCTOR

    template <typename number>
    data_manager_sqlite3<number>::~data_manager_sqlite3()
      {
        for(std::list<sqlite3*>::iterator t = this->open_containers.begin(); t != this->open_containers.end(); t++)
          {
            int status = sqlite3_close(*t);

            if(status != SQLITE_OK)
              {
                if(status == SQLITE_BUSY) throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, __CPP_TRANSPORT_DATACTR_NOT_CLOSED);
                else
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_DATACTR_CLOSE << status << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }
          }
      }


    // Create data files for a new integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::create_writer(typename repository<number>::integration_writer& ctr)
      {
        sqlite3* db = nullptr;
        sqlite3* taskfile = nullptr;

        // get paths of the data container and taskfile
        boost::filesystem::path ctr_path = ctr.data_container_path();
        boost::filesystem::path taskfile_path = ctr.taskfile_path();

        // open the main container

        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status != SQLITE_OK)
	        {
            std::ostringstream msg;
            if(db != nullptr)
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
	            }
            else
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ")";
	            }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
	        }

        sqlite3_extended_result_codes(db, 1);

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errmsg);

        // remember this connexion
        this->open_containers.push_back(db);
        ctr.set_data_manager_handle(db);

        // open the taskfile associated with this container

        status = sqlite3_open_v2(taskfile_path.string().c_str(), &taskfile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if(status != SQLITE_OK)
	        {
            sqlite3_close(db);

            std::ostringstream msg;
            if(taskfile != nullptr)
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << taskfile_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(taskfile) << ")";
                sqlite3_close(taskfile);
	            }
            else
	            {
                msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << taskfile_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ")";
	            }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
	        }

        sqlite3_extended_result_codes(taskfile, 1);

        // remember this connexion
        this->open_containers.push_back(taskfile);
        ctr.set_data_manager_taskfile(taskfile);
      }


    // Close data files associated with an integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(typename repository<number>::integration_writer& ctr)
      {
        // close sqlite3 handle to principal database
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(db);
        sqlite3_close(db);

        // close sqlite3 handle to taskfile
        sqlite3* taskfile = nullptr;
        ctr.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        this->open_containers.remove(taskfile);
        sqlite3_close(taskfile);

        // physically remove the taskfile from the disc; it isn't needed any more
        boost::filesystem::remove(ctr.taskfile_path());

        // physically remove the tempfiles directory
        boost::filesystem::remove(ctr.temporary_files_path());
      }


		// Create data files for a new derived_content_writer object
		template <typename number>
		void data_manager_sqlite3<number>::create_writer(typename repository<number>::derived_content_writer& ctr)
			{
				sqlite3* taskfile = nullptr;

				// get path to taskfile
		    boost::filesystem::path taskfile_path = ctr.taskfile_path();

				// open the taskfile

				int status = sqlite3_open_v2(taskfile_path.string().c_str(), &taskfile, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

				if(status != SQLITE_OK)
					{
				    std::ostringstream msg;
						if(taskfile != nullptr)
							{
								msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << taskfile_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(taskfile) << ")";
								sqlite3_close(taskfile);
							}
						else
							{
								msg << __CPP_TRANSPORT_DATACTR_CREATE_A << " '" << taskfile_path.string() << "' " << __CPP_TRANSPORT_DATACTR_CREATE_B << status << ")";
							}
						throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
					}

		    sqlite3_extended_result_codes(taskfile, 1);

				// remember this connexion
				this->open_containers.push_back(taskfile);
				ctr.set_data_manager_taskfile(taskfile);
			}


		// Close data files for a derived_content_writer object
		template <typename number>
		void data_manager_sqlite3<number>::close_writer(typename repository<number>::derived_content_writer& ctr)
			{
				// close sqlite3 handle to taskfile
				sqlite3* taskfile = nullptr;
				ctr.get_data_manager_taskfile(&taskfile);   // throws an exception if handle is unset, so this return value is guaranteed not to be nullptr

				this->open_containers.remove(taskfile);
				sqlite3_close(taskfile);

				// physically remove the taskfile from the disc; it isn't needed any more
//		    boost::filesystem::remove(ctr.get_taskfile_path());

				// physically remove the tempfiles directory
//		    boost::filesystem::remove(ctr.temporary_files_path());
			}


    // INDEX TABLE MANAGEMENT

    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_writer& ctr,
                                                     twopf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_dN_table(db, Nfields, sqlite3_operations::foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(typename repository<number>::integration_writer& ctr,
                                                     threepf_task<number>* tk, unsigned int Nfields)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_threepf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::real_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table(db, Nfields, sqlite3_operations::imag_twopf, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_threepf_table(db, Nfields, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_dN_table(db, Nfields, sqlite3_operations::foreign_keys);
//        sqlite3_operations::create_ddN_table(db, Nfields, sqlite3_operations::foreign_keys);
      }


    // TASKFILE MANAGEMENT

    // Create a tasklist based on a work queue of twopf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<twopf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        writer.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_taskfile(taskfile, queue);
      }

    // Create a tasklist based on a work queue of threepf_kconfig-s
    template <typename number>
    void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::integration_writer& writer, const work_queue<threepf_kconfig>& queue)
      {
        sqlite3* taskfile = nullptr;
        writer.get_data_manager_taskfile(&taskfile); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_taskfile(taskfile, queue);
      }

		// Create a tasklist based on a work queue of output_task_elements
		template <typename number>
		void data_manager_sqlite3<number>::create_taskfile(typename repository<number>::derived_content_writer& writer, const work_queue< output_task_element<number> >& queue)
			{
				sqlite3* taskfile = nullptr;
				writer.get_data_manager_taskfile(&taskfile);  // throws an exception if the handle is unset, so the return value is guaranteed not to be nullptr

		    sqlite3_operations::create_taskfile(taskfile, queue);
			}

    // Read a taskfile
    template <typename number>
    std::set<unsigned int> data_manager_sqlite3<number>::read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker)
      {
        return sqlite3_operations::read_taskfile(taskfile.string(), worker);
      }


    // TEMPORARY CONTAINERS

    template <typename number>
    typename data_manager<number>::twopf_batcher data_manager_sqlite3<number>::create_temp_twopf_container(const boost::filesystem::path& tempdir,
                                                                                                           const boost::filesystem::path& logdir,
                                                                                                           unsigned int worker,
                                                                                                           unsigned int Nfields,
                                                                                                           typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                                           boost::timer::cpu_timer& integration_timer)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::twopf_writer_group writers;
        writers.backg = std::bind(&sqlite3_operations::write_backg<number>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_twopf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::twopf_batcher batcher(this->capacity, Nfields, container, logdir, writers, dispatcher, replacer, db, worker, integration_timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "** Created new temporary twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }

    template <typename number>
    typename data_manager<number>::threepf_batcher data_manager_sqlite3<number>::create_temp_threepf_container(const boost::filesystem::path& tempdir,
                                                                                                               const boost::filesystem::path& logdir,
                                                                                                               unsigned int worker,
                                                                                                               unsigned int Nfields,
                                                                                                               typename data_manager<number>::container_dispatch_function dispatcher,
                                                                                                               boost::timer::cpu_timer& integration_timer)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

        // set up writers
        typename data_manager<number>::threepf_writer_group writers;
        writers.backg    = std::bind(&sqlite3_operations::write_backg<number>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_re = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::real_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_im = std::bind(&sqlite3_operations::write_twopf<number>, sqlite3_operations::imag_twopf, std::placeholders::_1, std::placeholders::_2);
        writers.threepf  = std::bind(&sqlite3_operations::write_threepf<number>, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        typename data_manager<number>::container_replacement_function replacer =
                                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_threepf_container,
                                                                                  this, tempdir, worker, Nfields,
                                                                                  std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        typename data_manager<number>::threepf_batcher batcher(this->capacity, Nfields, container, logdir, writers, dispatcher, replacer, db, worker, integration_timer);

        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << "** Created new temporary threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                    unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                                    typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == data_manager<number>::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_twopf_container(container, Nfields);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                      unsigned int Nfields, typename data_manager<number>::generic_batcher* batcher,
                                                                      typename data_manager<number>::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal)
            << "** " << (action == data_manager<number>::action_replace ? "Replacing" : "Closing")
            << " temporary threepf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "** Closed sqlite3 handle for " << batcher->get_container_path();

        if(action == data_manager<number>::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            BOOST_LOG_SEV(batcher->get_log(), data_manager<number>::normal) << "** Opening new threepf container " << container;

            sqlite3* new_db = sqlite3_operations::create_temp_threepf_container(container, Nfields);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    boost::filesystem::path data_manager_sqlite3<number>::generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker)
      {
        std::ostringstream container_name;
        container_name << __CPP_TRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << __CPP_TRANSPORT_TEMPORARY_CONTAINER_XTN;

        boost::filesystem::path container = tempdir / container_name.str();

        return(container);
      }


    template <typename number>
    void data_manager_sqlite3<number>::aggregate_twopf_batch(typename repository<number>::integration_writer& ctr,
                                                             const std::string& temp_ctr, model<number>* m, integration_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, ctr, temp_ctr, m, tk, sqlite3_operations::gauge_xfm_1);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::real_twopf);
      }


    template <typename number>
    void data_manager_sqlite3<number>::aggregate_threepf_batch(typename repository<number>::integration_writer& ctr,
                                                               const std::string& temp_ctr, model<number>* m, integration_task<number>* tk)
      {
        sqlite3* db = nullptr;
        ctr.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, ctr, temp_ctr, m, tk, sqlite3_operations::gauge_xfm_2);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::real_twopf);
        sqlite3_operations::aggregate_twopf<number>(db, ctr, temp_ctr, sqlite3_operations::imag_twopf);
        sqlite3_operations::aggregate_threepf<number>(db, ctr, temp_ctr);
      }


		// DATAPIPES


		template <typename number>
		typename data_manager<number>::datapipe data_manager_sqlite3<number>::create_datapipe(const boost::filesystem::path& logdir,
		                                                                                      const boost::filesystem::path& tempdir,
                                                                                          typename data_manager<number>::output_group_finder finder,
                                                                                          typename data_manager<number>::derived_content_dispatch_function dispatcher,
		                                                                                      unsigned int worker, boost::timer::cpu_timer& timer)
			{
		    // set up callback API
		    typename data_manager<number>::datapipe_attach_function attach = std::bind(&data_manager_sqlite3<number>::datapipe_attach, this,
		                                                                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		    typename data_manager<number>::datapipe_detach_function detach = std::bind(&data_manager_sqlite3<number>::datapipe_detach, this,
		                                                                               std::placeholders::_1);

		    typename data_manager<number>::datapipe_time_sample_function tsample = std::bind(&data_manager_sqlite3<number>::pull_time_sample, this,
		                                                                                     std::placeholders::_1, std::placeholders::_2,
                                                                                         std::placeholders::_3, std::placeholders::_4);

		    typename data_manager<number>::datapipe_twopf_kconfig_sample_function twopf_kcfg_ksample = std::bind(&data_manager_sqlite3<number>::pull_twopf_kconfig_sample, this,
		                                                                                                         std::placeholders::_1, std::placeholders::_2,
                                                                                                             std::placeholders::_3, std::placeholders::_4);

        typename data_manager<number>::datapipe_threepf_kconfig_sample_function threepf_kcfg_ksample = std::bind(&data_manager_sqlite3<number>::pull_threepf_kconfig_sample, this,
                                                                                                                 std::placeholders::_1, std::placeholders::_2,
                                                                                                                 std::placeholders::_3, std::placeholders::_4);

        typename data_manager<number>::datapipe_background_time_sample_function bsample = std::bind(&data_manager_sqlite3<number>::pull_background_time_sample, this,
                                                                                                    std::placeholders::_1, std::placeholders::_2,
                                                                                                    std::placeholders::_3, std::placeholders::_4,
                                                                                                    std::placeholders::_5);

        typename data_manager<number>::datapipe_twopf_time_sample_function twopf_tsample = std::bind(&data_manager_sqlite3<number>::pull_twopf_time_sample, this,
                                                                                                     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                                                                     std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                                                                                                     std::placeholders::_7);

        typename data_manager<number>::datapipe_threepf_time_sample_function threepf_tsample = std::bind(&data_manager_sqlite3<number>::pull_threepf_time_sample, this,
                                                                                                         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                                                                         std::placeholders::_4, std::placeholders::_5,
                                                                                                         std::placeholders::_6);

		    // set up datapipe
		    typename data_manager<number>::datapipe pipe(this->capacity, logdir, tempdir, worker, timer, finder, attach, detach, dispatcher,
		                                                 tsample, twopf_kcfg_ksample, threepf_kcfg_ksample,
		                                                 bsample, twopf_tsample, threepf_tsample);

				BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << "** Created new datapipe, cache capacity " << format_memory(this->capacity);

				return(pipe);
			}


    template <typename number>
    void data_manager_sqlite3<number>::pull_time_sample(typename data_manager<number>::datapipe* pipe,
                                                        const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample, unsigned int worker)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_time_sample(db, serial_numbers, sample, worker);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_kconfig_sample(typename data_manager<number>::datapipe* pipe, const std::vector<unsigned int>& serial_numbers,
                                                                 typename std::vector<typename data_manager<number>::twopf_configuration>& sample, unsigned int worker)
			{
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

		    sqlite3_operations::pull_twopf_kconfig_sample<number>(db, serial_numbers, sample, worker);
			}


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_kconfig_sample(typename data_manager<number>::datapipe* pipe, const std::vector<unsigned int>& serial_numbers,
                                                                   typename std::vector<typename data_manager<number>::threepf_configuration>& sample, unsigned int worker)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_threepf_kconfig_sample<number>(db, serial_numbers, sample, worker);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_background_time_sample(typename data_manager<number>::datapipe* pipe,
                                                                   unsigned int id, const std::vector<unsigned int>& t_serials,
                                                                   std::vector<number>& sample, unsigned int worker)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_background_time_sample(db, id, t_serials, sample, worker);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_time_sample(typename data_manager<number>::datapipe* pipe,
                                                              unsigned int id, const std::vector<unsigned int>& t_serials,
                                                              unsigned int k_serial, std::vector<number>& sample,
                                                              typename data_manager<number>::datapipe_twopf_type type, unsigned int worker)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_twopf_time_sample(db, id, t_serials, k_serial, sample,
                                                   (type == data_manager<number>::twopf_real ? sqlite3_operations::real_twopf : sqlite3_operations::imag_twopf),
                                                   worker);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_time_sample(typename data_manager<number>::datapipe* pipe,
                                                                unsigned int id, const std::vector<unsigned int>& t_serials,
                                                                unsigned int k_serial, std::vector<number>& sample, unsigned int worker)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_threepf_time_sample(db, id, t_serials, k_serial, sample, worker);
	    }


    template <typename number>
    typename repository<number>::template output_group<typename repository<number>::integration_payload>
    data_manager_sqlite3<number>::datapipe_attach(typename data_manager<number>::datapipe* pipe,
                                                  typename data_manager<number>::output_group_finder& finder,
                                                  integration_task<number>* tk, const std::list<std::string>& tags)
			{
				assert(pipe != nullptr);
				if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

        assert(tk != nullptr);
        if(tk == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_PIPE_NULL_TASK);

				sqlite3* db = nullptr;

        // find a suitable output group for this task
        typename repository<number>::template output_group< typename repository<number>::integration_payload > group = finder(tk, tags);

        typename repository<number>::integration_payload& payload = group.get_payload();

				// get path to the output group data container
		    boost::filesystem::path ctr_path = group.get_repo_root_path() / payload.get_container_path();

				int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

				if(status != SQLITE_OK)
					{
				    std::ostringstream msg;
						if(db != nullptr)
							{
								msg << __CPP_TRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_OPEN_B << status << ": " << sqlite3_errmsg(db) << ")";
								sqlite3_close(db);
							}
						else
							{
								msg << __CPP_TRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << __CPP_TRANSPORT_DATACTR_OPEN_B << status << ")";
							}
						throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
					}

        sqlite3_extended_result_codes(db, 1);

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errmsg);

		    // force temporary databases to be stored in memory, for speed
		    sqlite3_exec(db, "PRAGMA temp_store = 2;", nullptr, nullptr, &errmsg);

        // remember this connexion
				this->open_containers.push_back(db);
				pipe->set_manager_handle(db);

				BOOST_LOG_SEV(pipe->get_log(), data_manager<number>::normal) << "** Attached sqlite3 container '" << ctr_path.string() << "' to datapipe";

        return(group);
			}


		template <typename number>
		void data_manager_sqlite3<number>::datapipe_detach(typename data_manager<number>::datapipe* pipe)
			{
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE);

				sqlite3* db = nullptr;
				pipe->get_manager_handle(&db);
				this->open_containers.remove(db);
				sqlite3_close(db);

				BOOST_LOG_SEV(pipe->get_log(), data_manager<number>::normal) << "** Detached sqlite3 container from datapipe";
			}


    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER

    template <typename number>
    data_manager<number>* data_manager_factory(unsigned int capacity)
      {
        return new data_manager_sqlite3<number>(capacity);
      }


  }   // namespace transport



#endif //__data_manager_sqlite3_H_
