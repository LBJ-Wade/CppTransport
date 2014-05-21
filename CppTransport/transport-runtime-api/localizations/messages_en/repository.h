//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H


#define __CPP_TRANSPORT_REPO_MISSING_ROOT                "Repository error: Root directory does not exist"
#define __CPP_TRANSPORT_REPO_MISSING_DATABASE            "Repository error: Repository database not present or inaccessible"
#define __CPP_TRANSPORT_REPO_PACKAGE_EXISTS              "Repository error: Attempt to insert duplicate model specification"
#define __CPP_TRANSPORT_REPO_TASK_EXISTS                 "Repository error: Attempt to insert duplicate integration specification"
#define __CPP_TRANSPORT_REPO_INSERT_ERROR                "Repository error: Error while inserting document into repository (backend code="
#define __CPP_TRANSPORT_REPO_QUERY_ERROR                 "Repository error: Error while querying document from repository (backend code="
#define __CPP_TRANSPORT_REPO_DELETE_ERROR                "Repository error: Error while deleting repository document (backend id="
#define __CPP_TRANSPORT_REPO_COMMIT_ERROR                "Repository error: Error while committing changes to repository (backend id="
#define __CPP_TRANSPORT_REPO_PRECOMMIT_ERROR             "Repository error: Error while preparing to commit changes to repository (backend id="

#define __CPP_TRANSPORT_REPO_ROOT_EXISTS                 "Repository error: Root directory already exists"
#define __CPP_TRANSPORT_REPO_WRITE_SLAVE                 "Internal error: Attempt to write to repository from MPI slave process"
#define __CPP_TRANSPORT_REPO_NOT_SET                     "Internal error: Attempt to access repository when unset in 'task_manager'"
#define __CPP_TRANSPORT_REPO_NULL_MODEL                  "Internal error: Null model when communicating with repository"
#define __CPP_TRANSPORT_REPO_NULL_TASK                   "Internal error: Null task when communicating with repository"
#define __CPP_TRANSPORT_REPO_MISSING_TASK                "Repository error: Could not find specified task"
#define __CPP_TRANSPORT_REPO_MISSING_PACKAGE             "Repository error: Could not find specified package"
#define __CPP_TRANSPORT_REPO_DUPLICATE_TASK              "Repository error: Duplicate task"
#define __CPP_TRANSPORT_REPO_DUPLICATE_PACKAGE           "Repository error: Duplicate package"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_A             "Repository error: Could not find model"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_B             "corresponding to specified task"

#define __CPP_TRANSPORT_REPO_SKIPPING_TASK               "; skipping this task"
#define __CPP_TRANSPORT_REPO_NONE                        "Nothing to do: no repository specified"
#define __CPP_TRANSPORT_RUN_REPAIR                       "; consider checking database integrity"
#define __CPP_TRANSPORT_REPO_FOR_TASK                    "for task"
#define __CPP_TRANSPORT_UNKNOWN_JOB_TYPE                 "Internal error: Unexpected job type in 'task_manager'"
#define __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE   "Repository error: Attempt to read an unset 'data_manager' data-container handle in 'integration_writer'"
#define __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETTASK     "Repository error: Attempt to read an unset 'data_manager' taskfile handle in 'integration_writer'"
#define __CPP_TRANSPORT_REPO_DERIVED_WRITER_UNSETTASK    "Repository error: Attempt to read an unset 'data_manager' taskfile handle in 'derived_content_writer'"

#define __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_EXISTS      "Repository error: Attempt to insert duplicate derived-product specification"
#define __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN "Repository error: Attempt to enumerate derived-product specifications for non integration task"

#define __CPP_TRANSPORT_REPO_MISSING_DERIVED_PRODUCT     "Repository error: Could not find specified derived-product"
#define __CPP_TRANSPORT_REPO_DUPLICATE_DERIVED_PRODUCT   "Repository error: Duplicate derived-product"

#define __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN     "Repository error: task named in output task is not of integration-type"
#define __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS   "Repository error: no matching output groups for task"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
