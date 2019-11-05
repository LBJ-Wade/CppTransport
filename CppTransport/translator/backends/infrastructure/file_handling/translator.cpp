//
// Created by David Seery on 09/12/2013.
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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include <assert.h>

#include <vector>

#include "translator.h"
#include "buffer.h"

#include "formatter.h"


class TemplateJanitor
  {
    
    // CONSTRUCTOR, DESTRUCTOR
    
  public:
    
    //! constructor
    TemplateJanitor(std::unique_ptr<std::ifstream> f, output_stack& os)
      : inf(std::move(f)),
        out_stack(os),
        closed(false)
      {
      }
    
    //! destructor
    ~TemplateJanitor()
      {
        if(!closed) this->close();
      }
    
    
    // INTERFACE
    
  public:
    
    void close()
      {
        this->inf->close();
        this->out_stack.pop();
        this->closed = true;
      }
    
    std::ifstream& get()
      {
        return *this->inf;
      }
    
    std::ifstream& operator*()
      {
        return this->get();
      }
    
    
    // INTERNAL DATA
    
  public:
    
    //! ifstream object -- we assume ownership of it
    std::unique_ptr<std::ifstream> inf;
    
    //! output stack
    output_stack& out_stack;
    
    //! flag to indicate whether we have already cleaned up
    bool closed;
    
  };


translator::translator(translator_data& payload)
  : data_payload(payload)
  {
    cache = std::make_unique<expression_cache>();
  }


translator::~translator()
	{
	  if(!this->data_payload.get_argument_cache().show_profiling()) return;

    auto hits = this->cache->get_hits();
    auto misses = this->cache->get_misses();
    
    double hit_rate = static_cast<double>(hits) / (static_cast<double>(hits) + static_cast<double>(misses));
    
    std::ostringstream expr_cache_msg;
    expr_cache_msg << hits << " " << (hits == 1 ? MESSAGE_EXPRESSION_CACHE_HIT : MESSAGE_EXPRESSION_CACHE_HITS)
                   << ", " << this->cache->get_misses() << " " << MESSAGE_EXPRESSION_CACHE_MISSES;
    auto prec = expr_cache_msg.precision();
    expr_cache_msg.precision(3);
    expr_cache_msg << " (" << 100.0*hit_rate << "%)";
    expr_cache_msg.precision(prec);
    expr_cache_msg << " (" << MESSAGE_EXPRESSION_CACHE_QUERY_TIME << " " << format_time(this->cache->get_query_time())
                   << ", " << MESSAGE_EXPRESSION_CACHE_INSERT_TIME << " " << format_time(this->cache->get_insert_time())
                   << ")";

    this->print_advisory(expr_cache_msg.str());
	}


void translator::print_advisory(const std::string& msg)
	{
    this->data_payload.message(msg);
	}


unsigned int translator::translate(const std::string& in, const error_context& ctx, const boost::filesystem::path& out, process_type type, filter_function* filter)
  {
		buffer buf(out);

    unsigned int rval = this->translate(in, ctx, buf, type, filter);

    return(rval);
  }


unsigned int translator::translate(const std::string& in, const error_context& ctx, buffer& buf, process_type type, filter_function* filter)
  {
    boost::optional< boost::filesystem::path > template_in;
    finder& path = this->data_payload.get_finder();

		// try to find a template corresponding to the input filename
    
    template_in = path.find(in + ".h");
    if(template_in) return this->process(*template_in, buf, type, filter);
    
    template_in = path.find(in);
    if(template_in) return this->process(*template_in, buf, type, filter);

    if (type == process_type::process_sampling_ini)
    {
      template_in = path.find(in + ".ini");
      if(template_in) return this->process(*template_in, buf, type, filter);
    }

    if (type == process_type::process_sampling_txt)
    {
      template_in = path.find(in + ".txt");
      if(template_in) return this->process(*template_in, buf, type, filter);
    }

    if (type == process_type::process_sampling_getdist_python)
    {
      template_in = path.find(in + ".py");
      if(template_in) return this->process(*template_in, buf, type, filter);
    }

    if (type == process_type::process_sampling_getdist_latex)
    {
      template_in = path.find(in + ".txt");
      if(template_in) return this->process(*template_in, buf, type, filter);
    }

    
    std::ostringstream msg;
    msg << ERROR_MISSING_TEMPLATE << " '" << in << ".h'";

    ctx.error(msg.str());
    
    return 0;
  }


unsigned int translator::process(const boost::filesystem::path& in, buffer& buf, process_type type, filter_function* filter)
  {
    std::unique_ptr<backend_data> backend;
    std::unique_ptr<tensor_factory> factory;
    std::unique_ptr<package_group> package;

    try
      {
        std::tie(backend, factory, package) = this->build_agents(in);
      }
    catch(std::runtime_error& xe)
      {
        // errors will already have been reported, so simply return
        return 0;
      }

    // open template, and return if there is an error
    auto inf = this->open_template(in, buf);
    if(!inf) return 0;

    // generate a macro replacement agent based on this package group; can assume that
    // package is not an empty pointer
    macro_agent agent(this->data_payload, *package, BACKEND_MACRO_PREFIX, BACKEND_LINE_SPLIT_EQUAL, BACKEND_LINE_SPLIT_SUM_EQUAL);

    // push this input file to the top of the filestack
    output_stack& os = this->data_payload.get_stack();
    os.push(in, buf, agent, type);  // current line number is automatically set to 2 (accounting for the header line)
    
    // set up janitor object to clean up if an exception is encountered during translation
    TemplateJanitor j(std::move(inf), os);

    // are we annotating the translated template?
    bool annotate = this->data_payload.annotate();

    unsigned int replacements = 0;

    while(!(*j).eof() && !(*j).fail())
      {
        replacements += this->process_line(*j, *package, agent, buf, os, filter, annotate);
        os.increment_line();
      }

    // report end of input to the backend;
    // this enables it to do any tidying-up which may be required,
    // such as depositing temporaries to a temporary pool
    package->report_end_of_input();

    // emit advisory that translation is complete
    std::ostringstream finished_msg;
    finished_msg << MESSAGE_TRANSLATION_RESULT << " " << replacements << " " << MESSAGE_REPLACEMENT_RULE_EXPANSIONS;
    this->data_payload.message(finished_msg.str());

    // report time spent doing macro replacement
    // package will also report on time and memory use when it goes out of scope and is destroyed
    package->report_macro_metadata(agent.get_total_work_time(), agent.get_tokenization_time());

    // close janitor object
    j.close();
    
    return(replacements);
  }


std::tuple< std::unique_ptr<backend_data>, std::unique_ptr<tensor_factory>, std::unique_ptr<package_group> >
translator::build_agents(const boost::filesystem::path& in)
  {
    // decide which backend and API version are required
    // backend_data will issue an error if the file doesn't exist, can't be opened,
    // or the header cannot be read correctly
    auto backend
      = std::make_unique<backend_data>(in, this->data_payload.get_stack(), this->data_payload.get_error_handler(),
                                       this->data_payload.get_warning_handler(),
                                       this->data_payload.get_finder(), this->data_payload.get_argument_cache());

    // bail out on failure; error messages will already have been issued
    if(!backend) throw std::runtime_error("");

    // ask backend to validate itself: minimum version is OK, model type matches translator;
    // error messages are issued, so no need to do anything here
    if(!backend->validate(this->data_payload)) throw std::runtime_error("");

    // from here on, can assume that template exists and can be read and handled by this version of CppTransport

    // Generate an appropriate tensor_factory instance

    // A backend consists of a set of macro replacement rules that collectively comprise a 'package group'.
    // The result is returned as a managed pointer, using std::unique_ptr<>
    auto factory = make_tensor_factory(this->data_payload, *this->cache);

    // try to synthesize an appropriate macro package group
    // for this backend, which depends on the language type declared int he
    // template header; will throw an exception if the language type is not understood
    std::unique_ptr<package_group> package;
    try
      {
        package = package_group_factory(in, *backend, this->data_payload, *factory);
        // the package group owns the lambda_manager, which performs much the same job for lambdas
        // as the expression_cache for concrete expressions.
        // However, they're treated differently because the lambda_manager needs a specific language
        // printer to do its job, so it has to be placed within the package group
      }
    catch(std::runtime_error& xe)
      {
        error_context err_context = this->data_payload.make_error_context();
        err_context.error(xe.what());
        throw;
      }

    return std::make_tuple(std::move(backend), std::move(factory), std::move(package));
  }


std::unique_ptr<std::ifstream> translator::open_template(const boost::filesystem::path& in, buffer& buf)
  {
    auto inf = std::make_unique<std::ifstream>();

    inf->open(in.string().c_str());
    if(!inf->is_open() || inf->fail())
      {
        std::ostringstream msg;
        msg << ERROR_READING_TEMPLATE << " " << in;

        error_context err_context = this->data_payload.make_error_context();
        err_context.error(msg.str());

        inf->close();
        return nullptr;
      }

    // skip over header line
    std::string line;
    std::getline(*inf, line);

    // emit advisory that translation is underway
    std::ostringstream translation_msg;
    translation_msg << MESSAGE_TRANSLATING << " '" << in.string() << "'";
    if(!buf.is_memory())
      {
        translation_msg << " " << MESSAGE_TRANSLATING_TO << " '" << buf.get_filename().string() << "'";
      }
    this->data_payload.message(translation_msg.str());

    return std::move(inf);
  }


unsigned int
translator::process_line(std::ifstream& inf, package_group& package, macro_agent& agent, buffer& buf, output_stack& os,
                         filter_function* filter, bool annotate)
  {
    // read in a line from the template
    std::string line;
    std::getline(inf, line);

    // apply macro replacement to this line, keeping track of how many replacements are performed;
    // result is supplied as a std::unique_ptr<> because we don't want to have to take copies
    // of a large array of strings
    unsigned int replacements = 0;
    std::unique_ptr< std::list<std::string> > line_list = agent.apply(line, replacements);

    if(line_list)
      {
        std::ostringstream continuation_tag;
        language_printer& printer = package.get_language_printer();

        continuation_tag << ANNOTATE_EXPANSION_OF_LINE << " " << os.get_line();

        unsigned int c = 0;
        for(const std::string& l : *line_list)
          {
            std::string out_line = l + (annotate && c > 0 ? " " + printer.comment(continuation_tag.str()) : "");

            if(filter != nullptr) buf.write_to_end((*filter)(out_line));
            else buf.write_to_end(out_line);

            ++c;
          }
      }

    return replacements;
  }
