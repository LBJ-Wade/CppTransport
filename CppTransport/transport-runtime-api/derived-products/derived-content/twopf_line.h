//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __twopf_line_H_
#define __twopf_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE      "twopf-components"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL      "real"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY "imaginary"



namespace transport
	{

    namespace derived_data
	    {

        //! general field twopf content producer, suitable
        //! for producing content usable in eg. a 2d plot or table.
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and twopf_line<>
		    template <typename number>
        class twopf_line: public virtual derived_line<number>
	        {

          public:

            //! which type of twopf are we deriving from?
            typedef enum { real, imaginary } twopf_type;

	          // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor
		        twopf_line(const twopf_list_task<number>& tk, index_selector<2>& sel, filter::twopf_kconfig_filter& kfilter);

		        //! Deserialization constructor
		        twopf_line(Json::Value& reader);

		        virtual ~twopf_line() = default;


            // MANAGE SETTINGS

          public:

            //! get twopf type setting
            twopf_type get_type() const { return(this->twopf_meaning); }
            //! set twopf type setting
            void set_type(twopf_type m) { this->twopf_meaning = m; }

            //! query type of twopf - is it real?
            bool is_real_twopf() const { return(this->twopf_meaning == real); }
            //! query type of twopf - is it imaginary?
            bool is_imag_twopf() const { return(this->twopf_meaning == imaginary); }


		        // LABELLING SERVICES

          public:

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label(unsigned int m, unsigned int n) const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label(unsigned int m, unsigned int n) const;


		        // K-CONFIGURATION SERVICES

          public:

		        //! lookup wavenumber axis data
		        void pull_wavenumber_axis(datapipe<number>& pipe, std::vector<double>& axis) const;


		        // WRITE TO A STREAM

          public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

          public:

		        //! Serialize this object
		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

          protected:

            //! record which indices are active in this group
            index_selector<2> active_indices;

            //! record which type of 2pf we are plotting
            twopf_type twopf_meaning;

	        };


		    // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
		    template <typename number>
		    twopf_line<number>::twopf_line(const twopf_list_task<number>& tk, index_selector<2>& sel, filter::twopf_kconfig_filter& kfilter)
		      : derived_line<number>(tk),
		        active_indices(sel), twopf_meaning(real)
			    {
		        if(active_indices.get_number_fields() != this->mdl->get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			              << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			              << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->mdl->get_N_fields() << ")";
		            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
			        }

				    // set up a list of serial numbers corresponding to the k-configurations for this derived line
            try
              {
                this->f.filter_twopf_kconfig_sample(kfilter, tk.get_twopf_kconfig_list(), this->kconfig_sample_sns);
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == runtime_exception::FILTER_EMPTY)
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_EMPTY_FILTER << " '" << this->get_parent_task()->get_name() << "'";
                    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
                  }
                else throw xe;
              }
			    }


		    // Deserialization constructor DOESN'T CALL the proper derived_line<> deserialization constructor
		    // because of virtual inheritance; concrete classes must call it themselves
		    template <typename number>
		    twopf_line<number>::twopf_line(Json::Value& reader)
		      : derived_line<number>(reader),
		        active_indices(reader)
			    {
		        std::string tpf_type = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE].asString();

		        if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL) twopf_meaning = real;
		        else if(tpf_type == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY) twopf_meaning = imaginary;
		        else
			        {
		            std::ostringstream msg;
		            msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN << " '" << tpf_type << "'";
		            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
			        }
			    }


		    template <typename number>
		    std::string twopf_line<number>::make_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        label << (this->twopf_meaning == real ? __CPP_TRANSPORT_LATEX_RE_SYMBOL : __CPP_TRANSPORT_LATEX_IM_SYMBOL) << " ";

		        const std::vector<std::string>& field_names = this->mdl->get_f_latex_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[m % N_fields] << (m >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			                << field_names[n % N_fields] << (n >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			        }
		        else
			        {
		            label << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
			                << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

		        return (label.str());
			    }


		    template <typename number>
		    std::string twopf_line<number>::make_non_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        label << (this->twopf_meaning == real ? __CPP_TRANSPORT_NONLATEX_RE_SYMBOL : __CPP_TRANSPORT_NONLATEX_IM_SYMBOL) << " ";

		        const std::vector<std::string>& field_names = this->mdl->get_field_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[m % N_fields] << (m >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
			                << field_names[n % N_fields] << (n >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
			        }
		        else
			        {
		            label << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
			                << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

				    return(label.str());
			    }


		    template <typename number>
		    void twopf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

		        switch(this->twopf_meaning)
			        {
		            case real:
			            {
		                writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL);
		                break;
			            }

		            case imaginary:
			            {
		                writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY);
		                break;
			            }

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN);
			        }
			    }


		    template <typename number>
		    void twopf_line<number>::pull_wavenumber_axis(datapipe<number>& pipe, std::vector<double>& axis) const
			    {
				    typename datapipe<number>::twopf_kconfig_handle& handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
				    twopf_kconfig_tag<number> tag = pipe.new_twopf_kconfig_tag();

            // safe to take a reference here and avoid a copy
				    const std::vector< twopf_configuration >& configs = handle.lookup_tag(tag);

				    axis.clear();
				    for(typename std::vector< twopf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
					    {
						    if(this->klabel_meaning == derived_line<number>::comoving) axis.push_back((*t).k_comoving);
						    else if(this->klabel_meaning == derived_line<number>::conventional) axis.push_back((*t).k_conventional);
						    else
							    {
						        assert(false);
						        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
							    }
					    }
			    }


		    template <typename number>
		    void twopf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TWOPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->mdl->get_state_names());
		        out << std::endl;
			    }


	    }   // namespace derived_data

	}   // namespace transport


#endif //__twopf_line_H_
