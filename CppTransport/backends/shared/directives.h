//
// Created by David Seery on 05/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_DIRECTIVES_H
#define CPPTRANSPORT_DIRECTIVES_H


#include "replacement_rule_package.h"

#include "macro_tokenizer.h"


namespace macro_packages
  {

    constexpr unsigned int SET_DIRECTIVE_NAME_ARGUMENT = 0;
    constexpr unsigned int SET_DIRECTIVE_DEFINITION_ARGUMENT = 1;
    constexpr unsigned int SET_DIRECTIVE_TOTAL_ARGUMENTS = 2;


    namespace directives_impl
      {

        class user_macro: public replacement_rule_index
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            user_macro(std::string n, std::unique_ptr<token_list> t, const abstract_index_list& i, error_context d)
              : replacement_rule_index(n, 0, i.size()),
                tokens(std::move(t)),
                indices(i),
                declaration_point(std::move(d))
              {
              }

            //! destructor
            virtual ~user_macro() = default;


            // INTERFACE

          public:

            //! determined unroll status, which is inherited from the unroll status of the token list we contain
            enum unroll_behaviour get_unroll() const override { return this->tokens->unroll_status(); }

            //! get declaration point
            const error_context& get_declaration_point() const { return(this->declaration_point); }


            // INTERNAL API -- implements a 'replacement_rule_index' interface

          protected:

            //! pre-hook required to call all pre-macros
            virtual void pre_hook(const macro_argument_list& args) override;

            //! post-hook, required to call post-hook of all tokens we contain
            //! should reset token list to pristine state for next evaluation
            virtual void post_hook(const macro_argument_list& args) override;

            //! unrolled evaluation
            virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

            //! roll-up evaluation
            virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


            // INTERNAL DATA

          private:

            //! tokenized version of macro definition
            std::unique_ptr<token_list> tokens;

            //! list of indices defining this macro
            abstract_index_list indices;

            //! record declaration point
            const error_context declaration_point;

          };

      }   // namespace directives_impl


    class set_directive: public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_directive(std::string n, translator_data& p, language_printer& prn)
          : replacement_rule_index(n, SET_DIRECTIVE_TOTAL_ARGUMENTS),
            payload(p),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_directive() = default;


        // INTERFACE

      public:

        //! determine unroll status; meaningless for a directive, so we always return 'prevent'
        //! in order to guarantee being provided with an abstract index list
        enum unroll_behaviour get_unroll() const override { return unroll_behaviour::prevent; }


        // INTERNAL API -- implements a 'replacement_rule_index' interface

      protected:

        //! evaluate unrolled; has no meaning here, so throws an exception
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! evaluate directive
        virtual std::string roll(const macro_argument_list& args, const abstract_index_list& indices) override;


        // INTERNAL API

      protected:

        //! validate indices discovered during tokenization against a supplied index list
        void validate_discovered_indices(const abstract_index_list& supplied, const abstract_index_list& discovered);


        // INTERNAL DATA

      private:

        //! reference to data payload provided by translator
        translator_data& payload;

        //! reference to language printer object
        language_printer& printer;

        typedef std::unordered_map< std::string, std::unique_ptr<directives_impl::user_macro> > macro_table;

        //! symbol table for macros
        macro_table macros;

      };

    class directives: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        directives(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor
        virtual ~directives() = default;

      };

  }   // namespace macro_packages




#endif //CPPTRANSPORT_DIRECTIVES_H
