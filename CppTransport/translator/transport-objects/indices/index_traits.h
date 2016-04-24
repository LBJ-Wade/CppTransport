//
// Created by David Seery on 19/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_INDEX_TRAITS_H
#define CPPTRANSPORT_INDEX_TRAITS_H


#include "indices.h"
#include "abstract_index.h"


class index_traits
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    index_traits(unsigned int nf)
      : num_fields(nf)
      {
      }

    //! destructor is default
    ~index_traits() = default;


    // INTERFACE -- CONCRETE INDEX INTROSPECTION

  public:

    //! determine if a concrete index is a species label
    bool is_species(const phase_index& index);

    //! determine if a concrete index is a momentum label
    bool is_momentum(const phase_index& index);


    // INTERFACE -- CONCRETE INDEX CONVERSION

  public:

    //! convert a concrete index to a concrete momentum index
    phase_index to_momentum(const field_index& index);

    //! convert a concrete index to a concrete momentum index
    phase_index to_momentum(const phase_index& index);

    //! convert a concrete index to a concrete species label
    field_index to_species(const phase_index& index);


    // INTERFACE -- ABSTRACT INDEX CONVERSION

  public:

    //! convert an abstract phase-space index to an abstract species index
    //! conversion is direct
    abstract_index species_to_species(const abstract_index& index);

    //! convert an abstract phase-space index to an abstract species index
    //! conversion is offset by -number_fields
    abstract_index momentum_to_species(const abstract_index& index);

    // INTERNAL DATA

  private:

    //! cache number of fields
    unsigned int num_fields;

  };


#endif //CPPTRANSPORT_INDEX_TRAITS_H
