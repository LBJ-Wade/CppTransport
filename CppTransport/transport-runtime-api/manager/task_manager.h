//
// Created by David Seery on 24/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __task_manager_H_
#define __task_manager_H_

#include <functional>

#include "transport-runtime-api/manager/instance_manager.h"
#include "transport-runtime-api/repository/json_repository.h"

#include "transport-runtime-api/manager/master_controller.h"
#include "transport-runtime-api/manager/slave_controller.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/ansi_colour_codes.h"


namespace transport
  {

    //! Task manager is a managed interface to CppTransport's integration and output stacks,
    //! relying on a JSON-aware repository interface 'json_repository'
    //! to handle storage and serialization, and MPI to handle task communication.
    template <typename number>
    class task_manager: public instance_manager<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a task manager using command-line arguments. The repository must exist and be named on the command line.
        task_manager(int argc, char* argv[],
                     unsigned int bcp=CPPTRANSPORT_DEFAULT_BATCHER_STORAGE,
                     unsigned int pcp=CPPTRANSPORT_DEFAULT_PIPE_STORAGE);

        //! Construct a task manager using a previously-constructed repository object. Usually this will be used only when creating a new repository.
        task_manager(int argc, char* argv[], std::shared_ptr< json_repository<number> > r,
                     unsigned int bcp=CPPTRANSPORT_DEFAULT_BATCHER_STORAGE,
                     unsigned int pcp=CPPTRANSPORT_DEFAULT_PIPE_STORAGE);

        //! Destroy a task manager object
        ~task_manager() = default;


		    // INTERFACE -- MANAGE WORK

      public:

		    //! process work
		    void process(void);


        // INTERFACE -- REPOSITORY MANAGEMENT


        // INTERFACE -- ERROR REPORTING

      protected:

        //! Report an error
        void error(const std::string& msg);

        //! Report a warning
        void warn(const std::string& msg);

        //! Report a message
        void message(const std::string& msg);


        // INTERNAL DATA

      protected:


		    // MPI ENVIRONMENT

        //! BOOST::MPI environment
        boost::mpi::environment environment;

        //! BOOST::MPI world communicator
        boost::mpi::communicator world;


		    // SLAVE AND MASTER COMPONENT MANAGERS

		    //! slave controller
		    slave_controller<number> slave;

		    //! master controller
		    master_controller<number> master;

      };


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[], unsigned int bcp, unsigned int pcp)
	    : instance_manager<number>(),
	      environment(argc, argv),
	    // note it is safe to assume environment and world have been constructed when the constructor for
	    // slave and master are invoked, because environment and world are declared
	    // prior to slave and master in the class declaration
	      slave(environment, world, this->model_finder_factory(),
	            std::bind(&task_manager<number>::error, this, std::placeholders::_1),
	            std::bind(&task_manager<number>::warn, this, std::placeholders::_1),
	            std::bind(&task_manager<number>::message, this, std::placeholders::_1),
	            bcp, pcp),
	      master(environment, world,
	             std::bind(&task_manager<number>::error, this, std::placeholders::_1),
	             std::bind(&task_manager<number>::warn, this, std::placeholders::_1),
	             std::bind(&task_manager<number>::message, this, std::placeholders::_1),
	             bcp, pcp)
      {
        if(world.rank() == MPI::RANK_MASTER)  // process command-line arguments if we are the master node
	        {
            master.process_arguments(argc, argv, *this);
	        }
      }


    template <typename number>
    task_manager<number>::task_manager(int argc, char* argv[], std::shared_ptr< json_repository<number> > r, unsigned int bcp, unsigned int pcp)
      : instance_manager<number>(),
        environment(argc, argv),
        // note it is safe to assume environment and world have been constructed when the constructor for
        // slave and master are invoked, because environment and world are declared
        // prior to slave and master in the class declaration
        slave(environment, world, this->model_finder_factory(),
              std::bind(&task_manager<number>::error, this, std::placeholders::_1),
              std::bind(&task_manager<number>::warn, this, std::placeholders::_1),
              std::bind(&task_manager<number>::message, this, std::placeholders::_1),
              bcp, pcp),
        master(environment, world, r,
               std::bind(&task_manager<number>::error, this, std::placeholders::_1),
               std::bind(&task_manager<number>::warn, this, std::placeholders::_1),
               std::bind(&task_manager<number>::message, this, std::placeholders::_1),
               bcp, pcp)
      {
        assert(r);

		    // set model finder for the repository
		    // (this is a function which, given a model ID, returns the model* instance representing it)
		    r->set_model_finder(this->model_finder_factory());
      }


		template <typename number>
		void task_manager<number>::process(void)
			{
				if(this->world.rank() == MPI::RANK_MASTER)
					{
            // output model list if it has been asked for (have to wait until this point to be sure all models have registered)
            if(this->master.get_arguments().get_model_list()) this->write_models(std::cout);
						this->master.execute_tasks();
					}
				else
					{
						this->slave.wait_for_tasks();
					}
			}


    template <typename number>
    void task_manager<number>::error(const std::string& msg)
      {
        bool colour = this->master.get_environment().get_terminal_colour_support() && this->master.get_arguments().get_colour_output();

        if(colour) std::cout << ANSI_BOLD_RED;
        std::cout << msg << '\n';
        if(colour) std::cout << ANSI_NORMAL;
      }


    template <typename number>
    void task_manager<number>::warn(const std::string& msg)
      {
        bool colour = this->master.get_environment().get_terminal_colour_support() && this->master.get_arguments().get_colour_output();

        if(colour) std::cout << ANSI_BOLD_MAGENTA;
        std::cout << CPPTRANSPORT_TASK_MANAGER_WARNING_LABEL << " ";
        if(colour) std::cout << ANSI_NORMAL;
        std::cout << msg << '\n';

      }


    template <typename number>
    void task_manager<number>::message(const std::string& msg)
      {
        bool colour = this->master.get_environment().get_terminal_colour_support() && this->master.get_arguments().get_colour_output();

        if(this->master.get_arguments().get_verbose())
          {
//            if(colour) std::cout << ANSI_GREEN;
            std::cout << msg << '\n';
//            if(colour) std::cout << ANSI_NORMAL;
          }
      }


  } // namespace transport


#endif //__task_manager_H_
