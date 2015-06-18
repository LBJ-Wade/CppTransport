//
// Created by David Seery on 05/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __task_helper_H_
#define __task_helper_H_

#include <string>
#include <memory>

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/concepts/initial_conditions.h"

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"
#include "transport-runtime-api/tasks/postintegration_tasks.h"

#include "transport-runtime-api/messages.h"

#include "sqlite3.h"


namespace transport
  {

    namespace integration_task_helper
      {

        template <typename number>
        integration_task<number>* deserialize(const std::string& nm, Json::Value& reader, sqlite3* handle, typename repository_finder<number>::package_finder& f)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            // extract initial conditions
            std::string pkg_name = reader[CPPTRANSPORT_NODE_PACKAGE_NAME].asString();
            std::unique_ptr< package_record<number> > record(f(pkg_name));
            initial_conditions<number> ics = record->get_ics();

            if(type == CPPTRANSPORT_NODE_TASK_TYPE_TWOPF)              return new twopf_task<number>(nm, reader, handle, ics);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC) return new threepf_cubic_task<number>(nm, reader, handle, ics);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_FLS)   return new threepf_fls_task<number>(nm, reader, handle, ics);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace integration_task_helper

    namespace output_task_helper
      {

        template <typename number>
        output_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::derived_product_finder& pfinder)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            if(type == CPPTRANSPORT_NODE_TASK_TYPE_OUTPUT) return new output_task<number>(nm, reader, pfinder);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace output_task_helper

    namespace postintegration_task_helper
      {

        template <typename number>
        postintegration_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& f)
          {
            std::string type = reader[CPPTRANSPORT_NODE_TASK_TYPE].asString();

            if     (type == CPPTRANSPORT_NODE_TASK_TYPE_ZETA_TWOPF)   return new zeta_twopf_task<number>(nm, reader, f);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_ZETA_THREEPF) return new zeta_threepf_task<number>(nm, reader, f);
            else if(type == CPPTRANSPORT_NODE_TASK_TYPE_FNL)          return new fNL_task<number>(nm, reader, f);

            std::ostringstream msg;
            msg << CPPTRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
          }

      }   // namespace postintegration_task_helper

  }   // namespace transport


#endif //__task_helper_H_
