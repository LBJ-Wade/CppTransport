//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "core_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


core_group::core_group(translator_data& p, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  : package_group(p, CPP_COMMENT_SEPARATOR, cache),
    printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them automatically when we add this packages
    // to our list
    cse_worker = std::make_unique<cpp::cpp_cse>(0, this->printer, this->data_payload);

    // construct replacement rule packages
    auto f  = std::make_unique<macro_packages::fundamental>       (p, this->printer);
    auto ft = std::make_unique<macro_packages::flow_tensors>      (p, this->printer);
    auto lt = std::make_unique<macro_packages::lagrangian_tensors>(p, this->printer);
    auto ut = std::make_unique<macro_packages::utensors>          (p, this->printer);
    auto xf = std::make_unique<macro_packages::gauge_xfm>         (p, this->printer);
    auto tp = std::make_unique<macro_packages::temporary_pool>    (p, this->printer);
    auto cm = std::make_unique<cpp::core_macros>                  (p, this->printer);

    // register these packages and transfer their ownership
    this->push_back(std::move(cm));
    this->push_back(std::move(tp));
    this->push_back(std::move(ut));
    this->push_back(std::move(xf));
    this->push_back(std::move(lt));
    this->push_back(std::move(ft));
    this->push_back(std::move(f));
  }
