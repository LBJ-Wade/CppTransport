//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_ZETA2_H
#define CPPTRANSPORT_CANONICAL_ZETA2_H


#include <memory>

#include "concepts/zeta2.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace canonical
  {

    class canonical_zeta2: public zeta2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_zeta2(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                        boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
          : zeta2(),
            printer(p),
            cse_worker(cw),
            cache(c),
            res(r),
            shared(s),
            fl(f),
            traits(t),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_zeta2() = default;


        // INTERFACE -- IMPLEMENTS A 'zeta2' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k, GiNaC::symbol& k1,
                                                          GiNaC::symbol& k2, GiNaC::symbol& a) override;

        //! evaluate component of tensor
        virtual GiNaC::ex compute_component(phase_index i, phase_index j,
                                            GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a) override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<map_lambda> compute_lambda(const abstract_index& i, const abstract_index& j,
                                                           GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a) override;

        //! invalidate cache
        virtual void reset_cache() override { this->cached = false; }


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual enum unroll_behaviour get_unroll() override;


        // INTERNAL API

      private:

        //! cache symbols
        void cache_symbols();

        //! populate workspace
        void populate_workspace();

        //! compute field-field entry
        GiNaC::ex expr_field_field(GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j,
                                   GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a);

        //! compute field-momentum or momentum-field entry;
        //! field index is i, momentum index is j
        //! likewise, corresponding momenta are k1, k2 respectively
        GiNaC::ex expr_field_momentum(GiNaC::idx& i, GiNaC::idx& j,
                                      GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j,
                                      GiNaC::symbol& k, GiNaC::symbol& k1, GiNaC::symbol& k2, GiNaC::symbol& a);


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
        std::unique_ptr<symbol_list> derivs;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! epsilon
        GiNaC::ex eps;

        //! Planck mass
        GiNaC::symbol Mp;

        //! quantity p
        GiNaC::ex p;

        //! cache status
        bool cached;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_ZETA2_H