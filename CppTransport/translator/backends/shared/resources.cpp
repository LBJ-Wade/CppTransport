//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <functional>

#include "resources.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, f.get_resource_manager(), prn))


namespace macro_packages
  {

    resources::resources(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND(set_params, "RESOURCE_PARAMETERS"));
        pre_package.emplace_back(BIND(set_coordinates, "RESOURCE_COORDINATES"));
        pre_package.emplace_back(BIND(set_phase_flatten, "PHASE_FLATTEN"));
        pre_package.emplace_back(BIND(set_field_flatten, "FIELD_FLATTEN"));
        pre_package.emplace_back(BIND(release_flatteners, "RELEASE_FLATTENERS"));
        pre_package.emplace_back(BIND(set_dV, "RESOURCE_DV"));
        pre_package.emplace_back(BIND(set_ddV, "RESOURCE_DDV"));
        pre_package.emplace_back(BIND(set_dddV, "RESOURCE_DDDV"));
        pre_package.emplace_back(BIND(release, "RESOURCE_RELEASE"));
        pre_package.emplace_back(BIND(set_working_type, "WORKING_TYPE"));
        pre_package.emplace_back(BIND(release_working_type, "RELEASE_WORKING_TYPE"));
      }


    std::string set_params::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_coordinates::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_phase_flatten::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_field_flatten::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release_flatteners::evaluate(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return this->printer.comment(RESOURCE_RELEASE_FLATTENERS);
      }


    std::string set_dV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_dV(args[RESOURCES::DV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[RESOURCES::DV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_ddV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_ddV(args[RESOURCES::DDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[RESOURCES::DDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_dddV::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_dddV(args[RESOURCES::DDDV_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[RESOURCES::DDDV_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_connexion::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_connexion(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string set_Riemann::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_Riemann(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release::evaluate(const macro_argument_list& args)
      {
        this->mgr.release();

        return this->printer.comment(RESOURCE_RELEASE);
      }


    std::string set_working_type::evaluate(const macro_argument_list& args)
      {
        this->mgr.assign_working_type(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_WORKING_TYPE << " '" << static_cast<std::string>(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]) << "'";

        return this->printer.comment(msg.str());
      }


    std::string release_working_type::evaluate(const macro_argument_list& args)
      {
        this->mgr.release_working_type();

        return this->printer.comment(RESOURCE_RELEASE_WORKING_TYPE);
      }
  }