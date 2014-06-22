//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __fNL_line_H_
#define __fNL_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group_record
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE             "template"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL       "local"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL "equilateral"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL  "orthogonal"
#define __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI         "DBI"


namespace transport
  {

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class threepf_task;

    namespace derived_data
      {

        //! general fNL content producer
        //! As usual, we derive virtually from derived_line<>, so concrete
        //! classes must call its constructor explicitly
        template <typename number>
        class fNL_line: public virtual derived_line<number>
          {

          public:

            typedef enum { fNLlocal, fNLequi, fNLortho, fNLDBI } fNLtype;

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! Basic user-facing constructor
            fNL_line(const threepf_task<number>& tk);

            //! Deserialization constructor
            fNL_line(serialization_reader* reader);

            virtual ~fNL_line() = default;


            // LABELLING SERVICES

            //! make a LaTeX label for one of our lines
            std::string make_LaTeX_label() const;

            //! make a non-LaTeX label
            std::string make_non_LaTeX_label() const;


            // SET/GET FNL TEMPLATE

          public:

            //! get type of fNL
            fNLtype get_type() const { return(this->type); }

            //! set type of fNL
            void set_type(fNLtype t) { this->type = t; }


            // COMPUTE SELECTED FNL SHAPE FUNCTION

          public:

            //! compute shape function for fNL
            void fNL_shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                      const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                      std::vector<number>& shape) const;


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! Serialize this object
            virtual void serialize(serialization_writer& writer) const override;


            // SHAPE TEMPLATES

          protected:

            void local_template(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                std::vector<number>& shape) const;


            // INTERNAL DATA

          protected:

            //! record which fNL template we are using
            fNLtype type;

          };


        template <typename number>
        fNL_line<number>::fNL_line(const threepf_task<number>& tk)
          : type(fNLlocal)
          {
            // we could store all kconfiguration numbers we're going to use, but it's
            // more space efficient to just recover them at runtime
          }


        template <typename number>
        fNL_line<number>::fNL_line(serialization_reader* reader)
          : type(fNLlocal)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_READER);

            std::string type_str;
            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, type_str);

            if     (type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL)       type = fNLlocal;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL) type = fNLequi;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL)  type = fNLortho;
            else if(type_str == __CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI)         type = fNLDBI;
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE << " '" << type_str << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }
          }


        template <typename number>
        std::string fNL_line<number>::make_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNLlocal:
                  label << __CPP_TRANSPORT_LATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNLequi:
                  label << __CPP_TRANSPORT_LATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNLortho:
                  label << __CPP_TRANSPORT_LATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNLDBI:
                  label << __CPP_TRANSPORT_LATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        std::string fNL_line<number>::make_non_LaTeX_label() const
          {
            std::ostringstream label;

            switch(this->type)
              {
                case fNLlocal:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_LOCAL_SYMBOL;
                  break;

                case fNLequi:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_EQUI_SYMBOL;
                  break;

                case fNLortho:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_ORTHO_SYMBOL;
                  break;

                case fNLDBI:
                  label << __CPP_TRANSPORT_NONLATEX_FNL_DBI_SYMBOL;
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }

            return(label.str());
          }


        template <typename number>
        void fNL_line<number>::fNL_shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                std::vector<number>& shape) const
          {
            shape.clear();
            shape.resize(bispectrum.size());

            switch(this->type)
              {
                case fNLlocal:
                  return this->local_template(bispectrum, twopf_k1, twopf_k2, twopf_k3, shape);
                  break;

                case fNLequi:
                  assert(false);
                  break;

                case fNLortho:
                  assert(false);
                  break;

                case fNLDBI:
                  assert(false);
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }
          }


        template <typename number>
        void fNL_line<number>::local_template(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
                                              const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
                                              std::vector<number>& shape) const
          {
            for(unsigned int j = 0; j < bispectrum.size(); j++)
              {
                number form_factor = (6.0/5.0) * (twopf_k1[j]*twopf_k2[j] + twopf_k1[j]*twopf_k3[j] + twopf_k2[j]*twopf_k3[j]);

                shape[j] = bispectrum[j] / form_factor;
              }
          }


        template <typename number>
        void fNL_line<number>::serialize(serialization_writer& writer) const
          {
            switch(this->type)
              {
                case fNLlocal:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_LOCAL));
                  break;

                case fNLequi:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_EQUILATERAL));
                  break;

                case fNLortho:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_ORTHOGONAL));
                  break;

                case fNLDBI:
                  writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_FNL_TEMPLATE_DBI));
                  break;

                default:
                  assert(false);
                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
              }
          }


        template <typename number>
        void fNL_line<number>::write(std::ostream& out)
          {
            out << "FIXME" << std::endl;
          }

      }   // namespace derived_data

  }   // namespace transport


#endif //__fNL_line_H_
