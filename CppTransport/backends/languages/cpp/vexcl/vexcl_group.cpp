//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include "vexcl_group.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "vexcl_steppers.h"
#include "vexcl_kernels.h"
#include "resources.h"

#include "cpp_cse.h"
#include "cpp_printer.h"


vexcl_group::vexcl_group(translator_data& p, u_tensor_factory& factory)
  : package_group(p, factory)
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    l_printer = std::make_unique<cpp::cpp_printer>();
    cse_worker = std::make_unique<cpp::cpp_cse>(0, *this->l_printer, this->data_payload);

    // construct replacement rule packages
    this->add_package<macro_packages::fundamental>       (p, *l_printer);
    this->add_package<macro_packages::flow_tensors>      (p, *l_printer);
    this->add_package<macro_packages::lagrangian_tensors>(p, *l_printer);
    this->add_package<macro_packages::utensors>          (p, *l_printer);
    this->add_package<macro_packages::gauge_xfm>         (p, *l_printer);
    this->add_package<macro_packages::resources>         (p, *l_printer);
    this->add_package<macro_packages::temporary_pool>    (p, *l_printer);
    this->add_package<vexcl::vexcl_steppers>             (p, *l_printer);
    this->add_package<vexcl::vexcl_kernels>              (p, *l_printer);
  }
