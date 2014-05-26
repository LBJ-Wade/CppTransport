//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_line_H_
#define __derived_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/data_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME            "task-name"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE             "threepf-momenta"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE       "derivatives"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA          "momenta"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE          "k-label-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL  "conventional"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING      "comoving"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION            "precision"

#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE                "axis-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES          "time-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KCONFIG_SERIES       "kconfig-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE                "value-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF                   "correlation-function"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD              "background-field"
#define __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL                  "fNL"


// derived content types
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE                 "time-data-group-type"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_BACKGROUND           "background-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TWOPF                "twopf-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_THREEPF              "threepf-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_ZETA_TWOPF           "zeta-twopf-group"
#define __CPP_TRANSPORT_NODE_PRODUCT_TDATA_ZETA_THREEPF         "zeta-threepf-group"


namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    // forward-declare class task.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class twopf_list_task;


		namespace derived_data
			{

				//! content producer
				template <typename number>
				class derived_line: public serializable
					{

				  public:

				    typedef enum { derivatives, momenta } dot_type;
				    typedef enum { conventional, comoving } klabel_type;

						typedef enum { time_series, kconfig_series} axis_type;

				    typedef enum { background_field, correlation_function, fNL } value_type;

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Basic user-facing constructor
						derived_line(const integration_task<number>& tk, model<number>* m, axis_type at, value_type vt,
						             unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! Deserialization constructor
						derived_line(serialization_reader* reader, typename repository<number>::task_finder finder);

						// Override default copy constructor to perofrm a deep copy of the parent task
						derived_line(const derived_line<number>& obj);

						virtual ~derived_line();


				    // ADMIN

				    //! Get parent task
				    const task<number>* get_parent_task() const { return(this->parent_task); }

						//! Get axis type
						axis_type get_axis_type() const { return(this->x_type); }
						//! Get value type
						value_type get_value_type() const { return(this->y_type); }

				    //! get dot meaning
				    dot_type get_dot_meaning() const { return(this->dot_meaning); }
				    //! set dot meaning
				    void set_dot_meaning(dot_type t) { this->dot_meaning = t; }

				    //! get label meaning
				    klabel_type get_klabel_meaning() const { return(this->klabel_meaning); }
				    //! set label meaning
				    void set_klabel_meaning(klabel_type t) { this->klabel_meaning = t; }


				    // DATAPIPE HANDLING

				  public:

				    //! attach datapipe to output group
				    void attach(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) const;

				    //! detach datapipe from output group
				    void detach(typename data_manager<number>::datapipe& detach) const;


				    // DERIVE LINES

				    //! generate data lines for plotting
				    virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const = 0;


				    // CLONE

				    //! self-replicate
				    virtual derived_line<number>* clone() const = 0;


				    // WRITE TO A STREAM

				    //! write self-details to a stream
				    virtual void write(std::ostream& out);


						// SERIALIZATION -- implements a 'serializable' interface

				  public:

						//! Serialize this object
						virtual void serialize(serialization_writer& writer) const override;


				    // INTERNAL DATA


						//! Axis type
						axis_type x_type;

						//! Value type
						value_type y_type;

				    //! record task which corresponds to this derivation
				    integration_task<number>* parent_task;

				    //! record model used for this derivation
				    model<number>* mdl;

				    //! record meaning of momenta - do we interpret them as derivatives?
				    dot_type dot_meaning;

				    //! record meaning of k-labels -- are they conventional or comoving?
				    klabel_type klabel_meaning;

						//! record default plot precision
						unsigned int precision;

				    //! Wrapped output utility
				    wrapped_output wrapper;

					};


				template <typename number>
				derived_line<number>::derived_line(const integration_task<number>& tk, model<number>* m,
				                                   axis_type at, value_type vt, unsigned int prec)
					: x_type(at), y_type(vt), dot_meaning(momenta), klabel_meaning(conventional), precision(prec),
		        mdl(m), parent_task(dynamic_cast<integration_task<number>*>(tk.clone()))
					{
						assert(parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_INTEGRATION_TASK);
					}


				template <typename number>
				derived_line<number>::derived_line(serialization_reader* reader, typename repository<number>::task_finder finder)
					: parent_task(nullptr), mdl(nullptr)
					{
						assert(reader != nullptr);

				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_READER);

				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION, precision);

				    std::string parent_task_name;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME, parent_task_name);

				    // extract parent task and model
				    task<number>* tk = finder(parent_task_name, mdl);
				    if((parent_task = dynamic_cast< integration_task<number>* >(tk)) == nullptr)
					    throw runtime_exception(runtime_exception::REPOSITORY_ERROR, __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN);

				    std::string xtype;
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, xtype);
						if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES) x_type = time_series;
						else if(xtype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KCONFIG_SERIES) x_type = kconfig_series;
						else
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE << " '" << xtype << "'";
								throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
							}

				    std::string ytype;
						reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, ytype);
						if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF) y_type = correlation_function;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD) y_type = background_field;
						else if(ytype == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL) y_type = fNL;
						else
							{
						    std::ostringstream msg;
								msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE << " '" << ytype << "'";
								throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
							}

				    std::string dot_meaning_value;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, dot_meaning_value);

				    if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE) dot_meaning = derivatives;
				    else if(dot_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA) dot_meaning = momenta;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN << " '" << dot_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }

				    std::string label_meaning_value;
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, label_meaning_value);

				    if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL) klabel_meaning = conventional;
				    else if(label_meaning_value == __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING) klabel_meaning = comoving;
				    else
					    {
				        std::ostringstream msg;
				        msg << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN << " '" << label_meaning_value << "'";
				        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
					    }
					}


				template <typename number>
				derived_line<number>::derived_line(const derived_line<number>& obj)
					: dot_meaning(obj.dot_meaning), klabel_meaning(obj.klabel_meaning), mdl(obj.mdl),   // it's OK to shallow copy 'mdl'. Pointers to model instances are managed by the instance_manager
					  x_type(obj.x_type), y_type(obj.y_type), precision(obj.precision),
		        parent_task(dynamic_cast<integration_task<number>*>(obj.parent_task->clone()))
					{
						assert(this->parent_task != nullptr);

				    if(parent_task == nullptr)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_INTEGRATION_TASK);
					}


				template <typename number>
				derived_line<number>::~derived_line()
					{
						assert(this->parent_task != nullptr);
						delete this->parent_task;
					}


		    template <typename number>
		    void derived_line<number>::attach(typename data_manager<number>::datapipe& pipe, const std::list<std::string>& tags) const
			    {
		        pipe.attach(this->parent_task, tags);
			    }


		    template <typename number>
		    void derived_line<number>::detach(typename data_manager<number>::datapipe& pipe) const
			    {
		        pipe.detach();
			    }


				template <typename number>
				void derived_line<number>::serialize(serialization_writer& writer) const
					{
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TASK_NAME, this->parent_task->get_name());

				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_PRECISION, this->precision);


				    switch(this->x_type)
							{
						    case time_series:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TIME_SERIES));
									break;

						    case kconfig_series:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_XTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KCONFIG_SERIES));
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
							}

						switch(this->y_type)
							{
						    case correlation_function:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_CF));
									break;

						    case background_field:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BGFIELD));
									break;

						    case fNL:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_YTYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL));
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE);
							}

				    switch(this->dot_meaning)
					    {
				        case derivatives:
					        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE));
				        break;

				        case momenta:
					        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_DOT_MOMENTA));
				        break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN);
					    }

				    switch(this->klabel_meaning)
					    {
				        case conventional:
					        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL));
				        break;

				        case comoving:
					        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_TYPE, std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_KLABEL_COMOVING));
				        break;

				        default:
					        assert(false);
					        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
					    }
					}


				template <typename number>
				void derived_line<number>::write(std::ostream& out)
					{
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_LABEL << std::endl;
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME << " '" << this->parent_task->get_name() << ", ";
						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME << " '" << this->mdl->get_name()
								<< " [" << this->mdl->get_author() << " | " << this->mdl->get_tag() << "]" << std::endl;

						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_XTYPE << " ";
						switch(this->x_type)
							{
						    case time_series:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TIME_SERIES_LABEL << std::endl;
									break;

						    case kconfig_series:
									out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KCONFIG_SERIES_LABEL << std::endl;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE);
							}

						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_YTYPE << " ";
						switch(this->y_type)
							{
						    case correlation_function:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_CF_LABEL << std::endl;
									break;

						    case background_field:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_BGFIELD_LABEL << std::endl;
									break;

						    case fNL:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_FNL_LABEL << std::endl;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE);
							}


						out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING << " ";
						switch(this->dot_meaning)
							{
						    case derivatives:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE;
									break;

						    case momenta:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN);
							}

						out << ", " << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING << " ";
						switch(this->klabel_meaning)
							{
						    case comoving:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING;
									break;

						    case conventional:
							    out << __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL;
									break;

						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
							}
					}


			}   // derived_data


	}   // namespace transport


#endif //__derived_line_H_