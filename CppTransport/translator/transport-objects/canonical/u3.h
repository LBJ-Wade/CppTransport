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

#ifndef CPPTRANSPORT_CANONICAL_U3_H
#define CPPTRANSPORT_CANONICAL_U3_H


#include <memory>

#include "concepts/u3.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"

#include "A.h"
#include "B.h"
#include "C.h"



namespace canonical
  {

    class canonical_u3: public u3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_u3(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                     boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
          : u3(),
            A_agent(p, cw, c, r, s, tm, f, t),
            B_agent(p, cw, c, r, s, tm, f, t),
            C_agent(p, cw, c, r, s, tm, f, t),
            printer(p),
            cse_worker(cw),
            cache(c),
            shared(s),
            res(r),
            fl(f),
            traits(t),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_u3() = default;


        // INTERFACE -- IMPLEMENTS A 'C' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! evaluate component of tensor
        virtual GiNaC::ex compute_component(phase_index i, phase_index j, phase_index k,
                                            GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<map_lambda> compute_lambda(const abstract_index& i, const abstract_index& j, const abstract_index& k,
                                                           GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& k3, GiNaC::symbol& a) override;

        //! invalidate cache; a no-op here
        virtual void reset_cache() override { return; }


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual enum unroll_behaviour get_unroll() override;


        // INTERNAL DATA

      private:

        // CACHES

        //! reference to language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to expression cache
        expression_cache& cache;

        //! reference to resource object
        resources& res;

        //! reference to shared resources
        shared_resources& shared;


        // AGENTS

        //! index flattener
        index_flatten& fl;

        //! index introspection
        index_traits& traits;


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE

        //! delegate for computing A-tensor
        canonical_A A_agent;

        //! delegate for computing B-tensor
        canonical_B B_agent;

        //! delegate for computing C-tensor
        canonical_C C_agent;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_U3_H
