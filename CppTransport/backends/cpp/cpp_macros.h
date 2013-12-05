//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __macropackage_cpp_macros_H_
#define __macropackage_cpp_macros_H_


#include "macro.h"
#include "replacement_rule_package.h"


namespace cpp
  {

    class cpp_macros: public ::macro_packages::replacement_rule_package
      {
      public:
        cpp_macros(replacement_data& d, ginac_printer p,
                   std::string ds=OUTPUT_DEFAULT_STEPPER_STATE_NAME)
          : default_state(ds), ::macro_packages::replacement_rule_package(d, p)
          {
          }

        const std::vector<simple_rule> get_pre_rules();
        const std::vector<simple_rule> get_post_rules();
        const std::vector<index_rule>  get_index_rules();

      protected:
        std::string default_state;

        std::string replace_stepper      (const struct stepper& s, std::string state_name);

        std::string replace_backg_stepper(const std::vector<std::string>& args);
        std::string replace_pert_stepper (const std::vector<std::string>& args);

      };

  } // namespace cpp



#endif //__macropackage_cpp_macros_H_
