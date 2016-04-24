//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_OUTPUT_STACK_H
#define CPPTRANSPORT_OUTPUT_STACK_H


#include <string>
#include <deque>

#include "filestack.h"
#include "buffer.h"

#include "boost/filesystem/operations.hpp"


// forward reference to avoid circularity
class macro_agent;


enum class process_type { process_core, process_implementation };


class output_stack: public filestack_derivation_helper<output_stack>
  {

  public:

    class inclusion
      {

      public:

		    inclusion(const boost::filesystem::path i, unsigned int l, buffer& b, macro_agent& a, enum process_type t)
			    : in(std::move(i)),
			      line(l),
			      buf(b),
			      agent(a),
			      type(t)
			    {
			    }

      public:

        boost::filesystem::path in;
        unsigned int            line;
        buffer&                 buf;
		    macro_agent&            agent;
        enum process_type       type;

      };


		// CONSTRUCTOR, DESTRUCTOR

  public:

    // TODO: intended to be explicitly defaulted, but Intel compiler prior to v16 complains
    virtual ~output_stack()
      {
      }


		// INTERFACE - implements a 'filestack' interface

  public:

		// PUSH AND POP

    // push an object to the top of the stack
    void                      push          (const boost::filesystem::path in, buffer& buf, macro_agent& agent, enum process_type type);

    virtual void              pop           () override;

		// HANDLE LINE NUMBERS

    virtual void              set_line      (unsigned int line) override;
    virtual unsigned int      increment_line() override;
    virtual unsigned int      get_line      () const override;

		// STRINGIZE

    virtual std::string       write         (size_t level) const override;
    virtual std::string       write         () const override;


		// INTERFACE - specific to output_stack

  public:

    buffer&           top_buffer            ();
    macro_agent&      top_macro_package     ();
    enum process_type top_process_type      () const;


		// INTERNAL DATA

  protected:

    std::deque<struct inclusion> inclusions;

  };


#endif //CPPTRANSPORT_OUTPUT_STACK_H