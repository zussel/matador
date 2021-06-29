#ifndef MATADOR_TEMPLATE_PARSER_HPP
#define MATADOR_TEMPLATE_PARSER_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_command_factory.hpp"

#include "matador/utils/string_cursor.hpp"
#include "matador/utils/string.hpp"

#include <string>
#include <memory>

namespace matador {

class string_cursor;

namespace http {
namespace detail {

/// @cond MATADOR_DEV

class template_part;
class template_expression;
class multi_template_part;

OOS_HTTP_API std::string parse_token(string_cursor &cursor);
OOS_HTTP_API std::string parse_filepath(string_cursor &cursor);
OOS_HTTP_API bool parse_end_of_command_tag(string_cursor &cursor);
OOS_HTTP_API std::shared_ptr<template_filter> parse_filter(string_cursor &cursor);
OOS_HTTP_API std::shared_ptr<template_expression> parse_expression(string_cursor &cursor);
OOS_HTTP_API std::string parse_operator(string_cursor &cursor);

class OOS_HTTP_API template_parser
{
public:
  template_parser() = default;

  template < typename Function >
  std::shared_ptr<template_part> parse(string_cursor &cursor, Function &&on_command);

  enum on_command_result_t {
    FINISHED,
    INTERMEDIATE,
    NEXT_COMMAND
  };

private:
  enum state_t {
    UNKNOWN,
    STATIC_TEXT,
    INITIATE_TEMPLATE,
    PROCESS_COMMAND,
    PROCESS_COMMAND_FINISH,
    PROCESS_VARIABLE,
    PROCESS_VARIABLE_READ,
    PROCESS_VARIABLE_FINISH,
    PROCESS_FILTER,
    PROCESS_FILTER_FINISHED
  };

private:
  std::unique_ptr<multi_template_part> parts_;

  state_t state_ = UNKNOWN;
};

template<typename Function>
std::shared_ptr<template_part> template_parser::parse(string_cursor &cursor, Function &&on_command)
{
  parts_ = std::make_unique<multi_template_part>();

  state_ = STATIC_TEXT;
  std::string current_part;
  bool needs_next_char = true;
  // copy until next '{' is found
  char c = cursor.current_char();
  while (!is_eos(c)) {
    // find begin of variable/command tag
    if (c == '{' && state_ == STATIC_TEXT) {
      parts_->push_back(std::make_shared<detail::static_part>(current_part));
      current_part.clear();
      // initiate template handling
      state_ = INITIATE_TEMPLATE;
    } else if (c == '{' && state_ == INITIATE_TEMPLATE) {
      state_ = PROCESS_VARIABLE;
    } else if (c == '%' && state_ == INITIATE_TEMPLATE) {
      state_ = PROCESS_COMMAND;
    } else if (c == '|' && (state_ == PROCESS_VARIABLE_READ || state_ == PROCESS_FILTER_FINISHED)) {
      state_ = PROCESS_FILTER;
    } else if (c == '}' && (state_ == PROCESS_VARIABLE_READ || state_ == PROCESS_FILTER_FINISHED)) {
      // process variable
      state_ = PROCESS_VARIABLE_FINISH;
    } else if (c == '%' && state_ == PROCESS_COMMAND) {
      // process command
      state_ = PROCESS_COMMAND_FINISH;
    } else if (c == '}' && (state_ == PROCESS_VARIABLE_FINISH || state_ == PROCESS_COMMAND_FINISH)) {
      state_ = STATIC_TEXT;
    } else if (state_ == PROCESS_FILTER) {
      std::shared_ptr<template_filter> filter_ptr = detail::parse_filter(cursor);
      parts_->parts().back()->append_filter(filter_ptr);
      state_ = PROCESS_FILTER_FINISHED;
      needs_next_char = false;
    } else if (state_ == PROCESS_VARIABLE) {
      std::string var = detail::parse_token(cursor);
      parts_->push_back(std::make_shared<detail::variable_part>(var));
      state_ = PROCESS_VARIABLE_READ;
      needs_next_char = false;
    } else if (state_ == PROCESS_COMMAND) {
      std::string cmd = detail::parse_token(cursor);
      auto res = on_command(cmd, parts_);
      if (res == FINISHED) {
        return std::shared_ptr<template_part>(parts_.release());
//      } else if (res == INTERMEDIATE) {
      } else {
        if (res == NEXT_COMMAND) {
          auto cmdptr = detail::template_command_factory::instance().produce(cmd);
          parts_->push_back(cmdptr->parse(cursor));
        }
        c = cursor.current_char();
        continue;
      }
    } else if (state_ == STATIC_TEXT) {
      current_part += c;
    } else {
      // not expected state -> reset to STATIC_TEXT
      state_ = STATIC_TEXT;
      current_part += c;
    }

    if (needs_next_char) {
      c = cursor.next_char();
    } else {
      c = cursor.current_char();
      needs_next_char = true;
    }
  }

  parts_->push_back(std::make_shared<detail::static_part>(current_part));

  return std::shared_ptr<template_part>(parts_.release());
}

/// @endcond

}
}
}

#endif //MATADOR_TEMPLATE_PARSER_HPP
