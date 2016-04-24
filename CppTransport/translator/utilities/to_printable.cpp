//
// Created by David Seery on 26/06/2013.
// based on http://stackoverflow.com/questions/2417588/escaping-a-c-string
//


#include "to_printable.h"


std::string to_printable(const std::string& input, bool quote, bool allow_newlines)
  {
    std::string result = "";

    std::back_insert_iterator<std::string> out = std::back_inserter(result);

    if(quote) *out++ = '"';
    for(const char c : input)
      {
        if(!quote or (' ' <= c and c <= '~' and c != '\\' and c != '"') or (c == '\n' and allow_newlines))
          {
            *out++ = c;
          }
        else
          {
            *out++ = '\\';
            switch(c)
              {
                case '"':
                  *out++ = '"';
                  break;
                case '\\':
                  *out++ = '\\';
                  break;
                case '\t':
                  *out++ = 't';
                  break;
                case '\r':
                  *out++ = 'r';
                  break;
                case '\n':
                  *out++ = 'n';
                  break;
                default:
                  char const *const hexdig = "0123456789ABCDEF";
                  *out++ = 'x';
                  *out++ = hexdig[c >> 4];
                  *out++ = hexdig[c & 0xF];
              }
          }
      }
    if(quote) *out++ = '"';

    return (result);
  }