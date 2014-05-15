//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __plot_product_H_
#define __plot_product_H_


#include <functional>

#include "transport-runtime-api/derived-products/derived_product.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX        "log-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY        "log-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY        "abs-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX    "reverse-x"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY    "reverse-y"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX       "latex"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL      "x-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT "x-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL      "y-label"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT "y-label-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE       "title"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT  "title-text"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND      "legend"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS  "legend-position"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR   "top-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR   "bottom-right"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL   "bottom-left"
#define __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL   "top-left"

namespace transport
	{

		namespace derived_data
			{

			  //! A plot2d-line is a data line suitable for inclusion in a plot2d-product

		    template <typename number>
		    class plot2d_line
			    {

			    };

		    //! A plot2d-product is a specialization of a derived-product that
		    //! produces a plot of something against time.

		    template <typename number>
		    class plot2d_product: public derived_product<number>
			    {

		      public:

				    typedef enum { top_left, top_right, bottom_left, bottom_right } legend_pos;

				    typedef std::function<bool(double)> time_filter;

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

						//! Basic user-facing constructor
		        plot2d_product(const std::string& name, const std::string& filename, const integration_task<number>& tk)
		          : derived_product<number>(name, filename, tk)
			        {
			        }

				    //! Deserialization constructor
				    plot2d_product(const std::string& name, const integration_task<number>* tk, serialization_reader* reader)
				      : derived_product<number>(name, tk, reader)
					    {
						    // extract data from reader;
						    assert(reader != nullptr);

					      reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX, log_x);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY, log_y);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY, abs_y);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX, reverse_x);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY, reverse_y);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX, use_LaTeX);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL, x_label);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT, x_label_text);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL, y_label);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT, y_label_text);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE, title);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT, title_text);
				        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND, legend);

				        std::string leg_pos;
						    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, leg_pos);

						    if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL) position = top_left;
						    else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR) position = top_right;
						    else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL) position = bottom_left;
						    else if(leg_pos == __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR) position = bottom_right;
						    else
							    {
						        std::ostringstream msg;
								    msg << __CPP_TRANSPORT_PRODUCT_PLOT2D_UNKNOWN_LEG_POS << " '" << leg_pos << "'";
								    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
							    }
					    }

		        virtual ~plot2d_product() = default;


		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels() = 0;
		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() = 0;


		        // GET AND SET BASIC PLOT LABELS

		      public:

		        //! get logarithmic x-axis setting
		        bool get_log_x() const { return(this->log_x); }
		        //! set logarithmic x-axis setting
		        void set_log_x(bool g) { this->log_x = g; }

		        //! get logarithmic y-axis setting
		        bool get_log_y() const { return(this->log_y); }
		        //! set logarithmic y-axis setting
		        void set_log_y(bool g) { this->log_y = g; }

		        //! get reverse x-axis setting
		        bool get_reverse_x() const { return(this->reverse_x); }
		        //! set reverse x-axis setting
		        void set_reverse_x(bool g) { this->reverse_x = g; }

		        //! get reverse y-axis setting
		        bool get_reverse_y() const { return(this->reverse_y); }
		        //! set reverse y-axis setting
		        void set_reverse_y(bool g) { this->reverse_y = g; }

		        //! get abs-y-axis setting
		        bool get_abs_y() const { return(this->abs_y); }
		        //! set abs-y-axis setting
		        void set_abs_y(bool g) { this->abs_y = g; }

		        //! get default LaTeX labels setting
		        bool get_use_LaTeX() const { return(this->use_LaTeX); }
		        //! set default LaTeX labels setting
		        void set_use_LaTeX(bool g) { this->use_LaTeX = g; }

		        //! get use x-axis label setting
		        bool get_x_label() const { return(this->x_label); }
		        //! set use x-axis label setting
		        void set_x_label(bool g) { this->x_label = g; }
		        //! get x-axis label text
		        const std::string& get_x_label_text() const { return(this->x_label_text); }
		        //! set x-axis label text
		        void set_x_label_text(const std::string& text)
			        {
		            if(text.empty())  // assume intended to suppress x-axis label
			            {
		                this->x_label_text.clear();
		                this->x_label = false;
			            }
		            else
			            {
		                this->x_label_text = text;
		                this->x_label = true;
			            }
			        }
		        //! clear x-axis label text
		        void clear_x_label_text() { this->x_label_text.clear(); }

		        //! get use y-axis label setting
		        bool get_y_label() const { return(this->y_label); }
		        //! set use y-axis label setting
		        void set_y_label(bool g) { this->y_label = g; }
		        //! get y-axis label text
		        const std::string& get_y_label_text() const { return(this->y_label_text); }
		        //! set y-axis label text
		        void set_y_label_text(const std::string& text)
			        {
		            if(text.empty())  // assume intended to suppress y-axis label
			            {
		                this->y_label_text.clear();
		                this->y_label = false;
			            }
		            else
			            {
		                this->y_label_text = text;
		                this->y_label = true;
			            }
			        }
		        //! clear y-axis label text
		        void clear_y_label_text() { this->y_label_text.clear(); }

		        //! get use title setting
		        bool get_title() const { return(this->title); }
		        //! set use title setting
		        void set_title(bool g) { this->title = g; }
		        //! get title text
		        const std::string& get_title_text() const { return(this->title_text); }
		        //! set title text
		        void set_title_text(const std::string& text)
			        {
		            if(text.empty())    // assume intended to suppress the title
			            {
		                this->title_text.clear();
		                this->title = false;
			            }
		            else
			            {
		                this->title_text = text;
		                this->title = true;
			            }
			        }
				    //! clear title text
				    void clear_title_text() { this->title_text.clear(); }

				    //! get legend setting
				    bool get_legend() { return(this->legend); }
				    //! set legend setting
				    void set_legend(bool g) { this->legend = g; }

				    //! get legend position
				    legend_pos get_legend_position() { return(this->position); }
				    //! set legend position
				    void set_legend_position(legend_pos pos) { this->position = pos; }


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    virtual void serialize(serialization_writer& writer) const override;


				    // WRITE SELF TO A STANDARD STREAM

		      public:

				    void write(std::ostream& out);


		        // INTERNAL DATA

		      protected:

				    //! time filter function: used to decide which time serial numbers should be used
				    //! when producing a plot
				    time_filter filter;

				    // PLOT DETAILS

		        //! logarithmic x-axis?
		        bool log_x;

		        //! logarithmic y-axis?
		        bool log_y;

				    //! reverse x-axis?
				    bool reverse_x;

				    //! reverse y-axis?
				    bool reverse_y;

		        //! take absolute value on y-axis? (mostly useful with log-y plots)
		        bool abs_y;

		        //! use LaTeX default labels
		        bool use_LaTeX;

		        //! generate an x-axis label?
		        bool x_label;
		        std::string x_label_text;

		        //! generate a y-axis label?
		        bool y_label;
		        std::string y_label_text;

		        //! generate a title?
		        bool title;
		        std::string title_text;

		        //! add a legend?
		        bool legend;

				    //! location of legend
				    legend_pos position;

			    };


				template <typename number>
				void plot2d_product<number>::serialize(serialization_writer& writer) const
					{
						this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGX, this->log_x);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LOGY, this->log_y);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_ABSY, this->abs_y);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEX, this->reverse_x);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_REVERSEY, this->reverse_y);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LATEX, this->use_LaTeX);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL, this->x_label);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_XLABEL_TEXT, this->x_label_text);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL, this->y_label);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_YLABEL_TEXT, this->y_label_text);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE, this->title);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_TITLE_TEXT, this->title_text);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND, this->legend);

						switch(this->position)
							{
						    case top_left:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TL));
									break;

						    case top_right:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_TR));
							    break;

						    case bottom_left:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BL));
							    break;

						    case bottom_right:
							    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_POS, std::string(__CPP_TRANSPORT_NODE_PRODUCT_PLOT2D_LEGEND_BR));
							    break;

						    default:
							    assert(false);
							    throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_PRODUCT_INVALID_LEGEND_POSITION);
							}

						this->derived_product<number>::serialize(writer);
					}


				template <typename number>
				void plot2d_product<number>::write(std::ostream& out)
					{
						unsigned int count = 0;

				    this->wrap_list_item(out, this->log_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGX, count);
				    this->wrap_list_item(out, this->log_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LOGY, count);
				    this->wrap_list_item(out, this->abs_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_ABSY, count);
				    this->wrap_list_item(out, this->reverse_x, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEX, count);
				    this->wrap_list_item(out, this->reverse_y, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_REVERSEY, count);
				    this->wrap_list_item(out, this->use_LaTeX, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LATEX, count);
				    this->wrap_list_item(out, this->x_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_XLABEL, count);
						if(this->x_label)
							this->wrap_value(out, this->x_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_list_item(out, this->y_label, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_YLABEL, count);
						if(this->y_label)
							this->wrap_value(out, this->y_label_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_list_item(out, this->title, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_TITLE, count);
						if(this->title)
							this->wrap_value(out, this->title_text, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LABEL, count);
				    this->wrap_list_item(out, this->legend, __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND, count);

						if(this->legend)
							{
								if(count > 0) out << ", ";
								switch(this->position)
									{
								    case top_left:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_TL;
											break;
								    case top_right:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_TR;
											break;
								    case bottom_left:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_BL;
											break;
								    case bottom_right:
									    out << __CPP_TRANSPORT_PRODUCT_PLOT2D_LABEL_LEGEND_BR;
											break;
								    default:
									    assert(false);
									}
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__plot_product_H_