//
// Created by David Seery on 02/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_READ_H
#define CPPTRANSPORT_DATA_MANAGER_READ_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"
#include "transport-runtime/sqlite3/operations/data_traits.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        // Read worker information table
        worker_information_db read_worker_table(sqlite3* db)
          {
            std::ostringstream read_stmt;
            read_stmt << "SELECT workgroup, worker, backend, back_stepper, pert_stepper, back_abs_tol, back_rel_tol, pert_abs_tol, pert_rel_tol, hostname, os_name, os_version, os_release, architecture, cpu_vendor_id FROM " << CPPTRANSPORT_SQLITE_WORKERS_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, read_stmt.str().c_str(), read_stmt.str().length()+1, &stmt, nullptr));

            worker_information_db worker_db;

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    unsigned int workgroup   = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
                    unsigned int worker      = static_cast<unsigned int>(sqlite3_column_int(stmt, 1));

                    std::string backend      = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 2)));
                    std::string back_stepper = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 3)));
                    std::string pert_stepper = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 4)));
                    double back_abs_tol      = sqlite3_column_double(stmt, 5);
                    double back_rel_tol      = sqlite3_column_double(stmt, 6);
                    double pert_abs_tol      = sqlite3_column_double(stmt, 7);
                    double pert_rel_tol      = sqlite3_column_double(stmt, 8);
                    std::string hostname     = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 9)));
                    std::string os_name      = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 10)));
                    std::string os_version   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 11)));
                    std::string os_release   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 12)));
                    std::string architecture = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 13)));
                    std::string cpu_vendor   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14)), static_cast<unsigned int>(sqlite3_column_bytes(stmt, 14)));

                    worker_db.insert(std::make_pair(std::make_pair(workgroup, worker),
                                                    std::make_unique<worker_record>(workgroup, worker, backend, pert_stepper,
                                                                                         back_stepper, back_abs_tol, back_rel_tol,
                                                                                         pert_abs_tol, pert_rel_tol,
                                                                                         hostname, os_name, os_version, os_release,
                                                                                         architecture, cpu_vendor)));
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_WORKER_TABLE_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(worker_db);
          }


        // Read statistics table
        timing_db read_statistics_table(sqlite3* db)
          {
            std::ostringstream read_stmt;
            read_stmt << "SELECT kserial, integration_time, batch_time, steps, refinements, workgroup, worker FROM " << CPPTRANSPORT_SQLITE_STATS_TABLE << ";";

            sqlite3_stmt* stmt;
            check_stmt(db, sqlite3_prepare_v2(db, read_stmt.str().c_str(), read_stmt.str().length()+1, &stmt, nullptr));

            timing_db data;

            int status;
            while((status = sqlite3_step(stmt)) != SQLITE_DONE)
              {
                if(status == SQLITE_ROW)
                  {
                    unsigned int workgroup   = static_cast<unsigned int>(sqlite3_column_int(stmt, 5));
                    unsigned int worker      = static_cast<unsigned int>(sqlite3_column_int(stmt, 6));

                    unsigned int serial      = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));

                    boost::timer::nanosecond_type integration_time = static_cast<boost::timer::nanosecond_type>(sqlite3_column_int64(stmt, 1));
                    boost::timer::nanosecond_type batch_time       = static_cast<boost::timer::nanosecond_type>(sqlite3_column_int64(stmt, 2));

                    unsigned int steps       = static_cast<unsigned int>(sqlite3_column_int(stmt, 3));
                    unsigned int refinements = static_cast<unsigned int>(sqlite3_column_int(stmt, 4));

                    data.insert(std::make_pair(serial,
                                               std::make_unique<timing_record>(serial, integration_time, batch_time,
                                                                               steps, refinements, workgroup, worker)));
                  }
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATAMGR_STATISTICS_TABLE_READ_FAIL << status << ": " << sqlite3_errmsg(db) << ")";
                    sqlite3_finalize(stmt);
                    throw runtime_exception(exception_type::DATA_MANAGER_BACKEND_ERROR, msg.str());
                  }
              }

            check_stmt(db, sqlite3_finalize(stmt));

            return(data);
          }


      }

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_READ_H