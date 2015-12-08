//
// Created by David Seery on 06/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MACRO_TYPES_H
#define CPPTRANSPORT_MACRO_TYPES_H


#include <string>
#include <vector>


// use vector to allow index subscripting
typedef std::vector< std::string > macro_argument_list;

enum class simple_macro_type { pre, post };


#endif //CPPTRANSPORT_MACRO_TYPES_H
