//
// Created by David Seery on 22/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_CREATE_H
#define CPPTRANSPORT_REPOSITORY_CREATE_H


#include "transport-runtime-api/sqlite3/operations/repository_common.h"


namespace transport
  {

    namespace sqlite3_operations
      {

        void create_repository_tables(sqlite3* db)
          {
            std::ostringstream packages_stmt;
            packages_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_PACKAGE_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "path TEXT)";
            exec(db, packages_stmt.str());

            std::ostringstream i_tasks_stmt;
            i_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "("
              << "name    TEXT PRIMARY KEY, "
              << "package TEXT, "
              << "path    TEXT, "
              << "FOREIGN KEY(package) REFERENCES " << CPPTRANSPORT_SQLITE_PACKAGE_TABLE << "(name));";
            exec(db, i_tasks_stmt.str());

            std::ostringstream p_tasks_stmt;
            p_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "("
              << "name   TEXT PRIMARY KEY, "
              << "parent TEXT, "
              << "path   TEXT, "
              << "FOREIGN KEY(parent) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
            exec(db, p_tasks_stmt.str());

            std::ostringstream o_tasks_stmt;
            o_tasks_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "("
              << "name   TEXT PRIMARY KEY, "
              << "path   TEXT);";
            exec(db, o_tasks_stmt.str());

            std::ostringstream products_stmt;
            products_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_DERIVED_PRODUCTS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "path TEXT);";
            exec(db, products_stmt.str());

            std::ostringstream i_groups_stmt;
            i_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_INTEGRATION_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_INTEGRATION_TASKS_TABLE << "(name));";
            exec(db, i_groups_stmt.str());

            std::ostringstream p_groups_stmt;
            p_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_POSTINTEGRATION_TASKS_TABLE << "(name));";
            exec(db, p_groups_stmt.str());

            std::ostringstream o_groups_stmt;
            o_groups_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_OUTPUT_GROUPS_TABLE << "("
              << "name TEXT PRIMARY KEY, "
              << "task TEXT, "
              << "path TEXT, "
              << "FOREIGN KEY(task) REFERENCES " << CPPTRANSPORT_SQLITE_OUTPUT_TASKS_TABLE << "(name));";
            exec(db, o_groups_stmt.str());

            std::ostringstream o_reserved_stmt;
            o_reserved_stmt << "CREATE TABLE " << CPPTRANSPORT_SQLITE_RESERVED_CONTENT_NAMES_TABLE << "("
              << "name       TEXT PRIMARY KEY, "
              << "task       TEXT, "
              << "path       TEXT, "
              << "posix_time TEXT);";   // no obvious way to implement constraint on task names
            exec(db, o_reserved_stmt.str());
          }

      }   // namespace sqlite3_operations

  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_CREATE_H
