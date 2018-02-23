//
// Created by David Seery on 21/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H


#include "canonical/resource_manager.h"


namespace nontrivial_metric
  {
    
    class resource_manager : public ::curvature_resource_manager
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor is default
        resource_manager(bool dev_warn)
          : parameters_cache(dev_warn),
            coordinates_cache(dev_warn),
            dV_cache(dev_warn),
            ddV_cache(dev_warn),
            dddV_cache(dev_warn),
            phase_flatten_cache(dev_warn),
            field_flatten_cache(dev_warn),
            working_type_cache(dev_warn),
            connexion_cache(dev_warn),
            metric_cache(dev_warn),
            metric_inverse_cache(dev_warn),
            Riemann_A2_cache(dev_warn),
            Riemann_A3_cache(dev_warn),
            Riemann_B3_cache(dev_warn)
          {
          }
        
        //! destructor is default
        ~resource_manager() = default;
    
    
        // QUERY FOR RESOURCE LABELS
        
      public:
    
        //! get parameters label
        const boost::optional< contexted_value<std::string> >&
        parameters() override
          { return this->parameters_cache.find(); }
    
        //! get phase-space coordinates label
        //! if exact is false then the closest possible match is returned, if one is found
        const boost::optional< contexted_value<std::string> >&
        coordinates() override
          { return this->coordinates_cache.find(); }
    
        //! get V,i label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DV_INDICES>, contexted_value<std::string> > >
        dV(std::array<variance, RESOURCE_INDICES::DV_INDICES> v = { variance::none }, bool exact=true) override
          { return this->dV_cache.find(v, exact); }
    
        //! get V,ij label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDV_INDICES>, contexted_value<std::string> > >
        ddV(std::array<variance, RESOURCE_INDICES::DDV_INDICES> v = { variance::none, variance::none }, bool exact=true) override
          { return this->ddV_cache.find(v, exact); }
    
        //! get V,ijk label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::DDDV_INDICES>, contexted_value<std::string> > >
        dddV(std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v = { variance::none, variance::none, variance::none }, bool exact=true) override
          { return this->dddV_cache.find(v, exact); }
    
    
        //! get phase-space flattening function
        const boost::optional< contexted_value<std::string> >&
        phase_flatten() override
          { return this->phase_flatten_cache.find(); }
    
        //! get field-space flattening function
        const boost::optional< contexted_value<std::string> >&
        field_flatten() override
          { return this->field_flatten_cache.find(); }
    
    
        //! get working type
        const boost::optional< contexted_value<std::string> >&
        working_type() override
          { return this->working_type_cache.find(); }

        
        //! get connexion label
        const boost::optional< contexted_value<std::string> >&
        connexion() override
          { return this->connexion_cache.find(); }
    
        //! get metric label
        const boost::optional< contexted_value<std::string> >&
        metric() override
          { return this->metric_cache.find(); }
    
        //! get inverse metric label
        const boost::optional< contexted_value<std::string> >&
        metric_inverse() override
          { return this->metric_inverse_cache.find(); }
    
        //! get Riemann A2 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES>, contexted_value<std::string> > >
        Riemann_A2(std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v, bool exact=true) override
          { return this->Riemann_A2_cache.find(v, exact); }
    
        //! get Riemann A3 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES>, contexted_value<std::string> > >
        Riemann_A3(std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v, bool exact=true) override
          { return this->Riemann_A3_cache.find(v, exact); }
    
        //! get Riemann B3 label
        boost::optional< std::pair< std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES>, contexted_value<std::string> > >
        Riemann_B3(std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v, bool exact=true) override
          { return this->Riemann_B3_cache.find(v, exact); }
    
    
        // ASSIGN RESOURCE LABELS
    
      public:
    
        //! assign parameter resource label
        void assign_parameters(const contexted_value<std::string>& p) override
          { this->parameters_cache.assign(p); }
    
        //! assign phase-space coordinate resource label
        void assign_coordinates(const contexted_value<std::string>& c) override
          { this->coordinates_cache.assign(c); }
    
        //! assign V,i resource label
        void assign_dV(const contexted_value<std::string>& d,
                       std::array<variance, RESOURCE_INDICES::DV_INDICES> v) override
          { this->dV_cache.assign(d, v); }
    
        //! assign V,ij resource label
        void assign_ddV(const contexted_value<std::string>& d,
                        std::array<variance, RESOURCE_INDICES::DDV_INDICES> v) override
          { this->ddV_cache.assign(d, v); }
    
        //! assign V,ijk resource label
        void assign_dddV(const contexted_value<std::string>& d,
                         std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v) override
          { this->dddV_cache.assign(d, v); }
    
    
        //! assign phase-space flattening function
        void assign_phase_flatten(const contexted_value<std::string>& f) override
          { this->phase_flatten_cache.assign(f); }
    
        //! assign field-space flattening function
        void assign_field_flatten(const contexted_value<std::string>& f) override
          { this->field_flatten_cache.assign(f); }
    
    
        //! assign working type
        void assign_working_type(const contexted_value<std::string>& t) override
          { this->working_type_cache.assign(t); }

        
        //! assign connexion resource label
        void assign_connexion(const contexted_value<std::string>& c) override
          { this->connexion_cache.assign(c); }
    
        //! assign metric resource label
        void assign_metric(const contexted_value<std::string>& c) override
          { this->metric_cache.assign(c); }
    
        //! assign inverse resource label
        void assign_metric_inverse(const contexted_value<std::string>& c) override
          { this->metric_inverse_cache.assign(c); }
    
        //! assign Riemann A2 resource label
        void assign_Riemann_A2(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v) override
          { this->Riemann_A2_cache.assign(R, v); }
    
        //! assign Riemann A3 resource label
        void assign_Riemann_A3(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v) override
          { this->Riemann_A3_cache.assign(R, v); }
    
        //! assign Riemann B3 resource label
        void assign_Riemann_B3(const contexted_value<std::string>& R,
                               std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v) override
          { this->Riemann_B3_cache.assign(R, v); }
    
    
        // RELEASE INDIVIDUAL RESOURCE LABELS

      public:
    
        //! release parameter resource
        void release_parameter() override
          { this->parameters_cache.reset(); }
    
        //! release phase-space coordinate resource
        void release_coordinates() override
          { this->coordinates_cache.reset(); }
    
        //! release V,i resource
        void release_dV() override
          { this->dV_cache.reset(); }
    
        //! release V,ij resource
        void release_ddV() override
          { this->ddV_cache.reset(); }
    
        //! release V,ijk resource
        void release_dddV() override
          { this->dddV_cache.reset(); }
    
    
        //! release phase-space flattening function
        void release_phase_flatten() override
          { this->phase_flatten_cache.reset(); }
    
        //! release field-space flattening function
        void release_field_flatten() override
          { this->field_flatten_cache.reset(); }
    
    
        //! release working type
        void release_working_type() override
          { this->working_type_cache.reset(); }

        
        //! release connexion resource
        void release_connexion() override
          { this->connexion_cache.reset(); }
    
        //! release metric resource
        void release_metricn() override
          { this->metric_cache.reset(); }
    
        //! release connexion resource
        void release_metric_inverse() override
          { this->metric_inverse_cache.reset(); }
    
        //! release Riemann A2 resource
        void release_Riemann_A2() override
          { this->Riemann_A2_cache.reset(); }
    
        //! release Riemann A3 resource
        void release_Riemann_A3() override
          { this->Riemann_A3_cache.reset(); }
    
        //! release Riemann B3 resource
        void release_Riemann_B3() override
          { this->Riemann_B3_cache.reset(); }
    
    
        // GLOBAL RELEASE OF RESOURCE LABELS
  
      public:
    
        //! release all resources, but not flatteners (this is the most common use case;
        //! we wish to release resource labels at the end of a function, but not the flattener labels)
        void release() override;
    
        //! release flatteners
        void release_flatteners() override;
    
    
        // INTERNAL DATA
        
      protected:
    
        //! cache parameters resource label
        simple_resource<std::string> parameters_cache;
    
        //! cache parameters resource labels
        simple_resource<std::string> coordinates_cache;
    
        //! cache V, resource labels
        indexed_resource<RESOURCE_INDICES::DV_INDICES, std::string> dV_cache;
    
        //! cache V,ij resource labels
        indexed_resource<RESOURCE_INDICES::DDV_INDICES, std::string> ddV_cache;
    
        //! cache V,ijk resource labels
        indexed_resource<RESOURCE_INDICES::DDDV_INDICES, std::string> dddV_cache;
    
    
        //! cache flattening function - full phase-space coordinates
        simple_resource<std::string> phase_flatten_cache;
    
        //! cache flattening function - field space coordinates
        simple_resource<std::string> field_flatten_cache;
    
    
        //! cache working type
        simple_resource<std::string> working_type_cache;

        
        //! cache connexion resource labels
        simple_resource<std::string> connexion_cache;
        
        //! cache metric resource label
        simple_resource<std::string> metric_cache;
        
        //! cache inverse metric resource label
        simple_resource<std::string> metric_inverse_cache;
    
        //! cache Riemann A2 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_A2_INDICES, std::string> Riemann_A2_cache;
    
        //! cache Riemann A3 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_A3_INDICES, std::string> Riemann_A3_cache;
    
        //! cache Riemann B3 resource labels
        indexed_resource<RESOURCE_INDICES::RIEMANN_B3_INDICES, std::string> Riemann_B3_cache;
        
      };
  
  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCE_MANAGER_H
