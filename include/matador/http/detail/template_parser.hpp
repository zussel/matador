#ifndef MATADOR_TEMPLATE_PARSER_HPP
#define MATADOR_TEMPLATE_PARSER_HPP

#include "matador/http/detail/template_part.hpp"

#include <string>
#include <memory>

namespace matador {

class string_cursor;

namespace http {
namespace detail {

class template_part;
class multi_template_part;

std::string parse_token(string_cursor &cursor);
bool parse_end_of_command_tag(string_cursor &cursor);

class template_parser
{
public:
  template_parser() = default;

  std::shared_ptr<template_part> parse(string_cursor &cursor);
  std::shared_ptr<template_part> parse_until_command(string_cursor &cursor, const std::string &stop_cmd);

private:
  void handle_command(string_cursor &cursor, const std::string &cmd);

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
}
}
}

#endif //MATADOR_TEMPLATE_PARSER_HPP
