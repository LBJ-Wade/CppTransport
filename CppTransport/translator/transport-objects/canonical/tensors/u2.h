//
// Created by David Seery on 20/12/2015.
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

#ifndef CPPTRANSPORT_CANONICAL_U2_H
#define CPPTRANSPORT_CANONICAL_U2_H


#include <memory>

#include "concepts/tensors/u2.h"
#include "shared/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace canonical
  {
    
    class u2 : public ::u2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        u2(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
           boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t);

        //! destructor is default
        virtual ~u2() = default;


        // INTERFACE -- IMPLEMENTS A 'u2' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        std::unique_ptr<flattened_tensor>
        compute(const index_literal_list& indices, symbol_wrapper& k, symbol_wrapper& a) override;

        //! evaluate component of tensor
        GiNaC::ex
        compute_component(phase_index i, phase_index j, symbol_wrapper& k, symbol_wrapper& a) override;

        //! evaluate lambda for tensor
        std::unique_ptr<map_lambda>
        compute_lambda(const index_literal& i, const index_literal& j,
                       symbol_wrapper& k, symbol_wrapper& a) override;


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        unroll_state get_unroll(const index_literal_list& idx_list) override;


        // INTERFACE -- JANITORIAL API

        //! cache resources required for evaluation
        void pre_explicit(const index_literal_list& indices) override;

        //! cache resources required for evaluation on a lambda
        void pre_lambda();

        //! release resources
        void post() override;


        // INTERNAL API

      private:

        //! underlying symbolic expression
        GiNaC::ex expr_momentum_field(const GiNaC::idx& i, const GiNaC::idx& j, const GiNaC::ex& Vij,
                                      const GiNaC::ex& Vi, const GiNaC::ex& Vj,
                                      const GiNaC::ex& deriv_i, const GiNaC::ex& deriv_j, const symbol_wrapper& k,
                                      const symbol_wrapper& a);


        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to expression cache
        expression_cache& cache;

        //! reference to resource object
        resources& res;

        //! reference to shared resource object
        shared_resources& shared;


        // AGENTS

        //! index flattener
        index_flatten& fl;

        //! index introspection
        index_traits& traits;


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE AND CACHE

        //! list of momentum symbols
        std::unique_ptr<flattened_tensor> derivs;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! flattened ddV tensor
        std::unique_ptr<flattened_tensor> ddV;

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! epsilon
        GiNaC::ex eps;

        //! Planck mass
        symbol_wrapper Mp;

        //! cache status
        bool cached;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_U2_H
