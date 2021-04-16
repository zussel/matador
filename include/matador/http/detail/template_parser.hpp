#ifndef MATADOR_TEMPLATE_PARSER_HPP
#define MATADOR_TEMPLATE_PARSER_HPP

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

class template_part;
class template_expression;
class multi_template_part;

std::string parse_token(string_cursor &cursor);
std::string parse_filepath(string_cursor &cursor);
bool parse_end_of_command_tag(string_cursor &cursor);
std::shared_ptr<template_expression> parse_expression(string_cursor &cursor);
std::string parse_operator(string_cursor &cursor);

class template_parser
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
    PROCESS_VARIABLE_FINISH
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
    } else if (c == '}' && state_ == PROCESS_VARIABLE) {
      // process variable
      state_ = PROCESS_VARIABLE_FINISH;
    } else if (c == '%' && state_ == PROCESS_COMMAND) {
      // process command
      state_ = PROCESS_COMMAND_FINISH;
    } else if (c == '}' && (state_ == PROCESS_VARIABLE_FINISH || state_ == PROCESS_COMMAND_FINISH)) {
      state_ = STATIC_TEXT;
    } else if (state_ == PROCESS_VARIABLE) {
      std::string var = detail::parse_token(cursor);
      parts_->push_back(std::make_shared<detail::variable_part>(var));
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

    c = cursor.next_char();
  }

  parts_->push_back(std::make_shared<detail::static_part>(current_part));

  return std::shared_ptr<template_part>(parts_.release());
}
}
}
}

#endif //MATADOR_TEMPLATE_PARSER_HPP
