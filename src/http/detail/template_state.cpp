#include "matador/http/detail/template_state.hpp"
#include "matador/http/detail/template_parser.hpp"

#include "matador/utils/string_cursor.hpp"

#include <stdexcept>

namespace matador {
namespace http {
namespace detail {

std::shared_ptr<template_part> foreach_state::parse(string_cursor &cursor)
{
  // parse until closing tag 'endfor' is read
  cursor.skip_whitespace();

  std::string elem_name = detail::parse_token(cursor);

  cursor.skip_whitespace();

  std::string in = detail::parse_token(cursor);

  if (in != "in") {
    throw std::logic_error("expected keyword 'in' for for loop");
  }

  cursor.skip_whitespace();

  std::string list_name = detail::parse_token(cursor);

  if (!detail::parse_end_of_command_tag(cursor)) {
    throw std::logic_error("couldn't read expected command end tag '%}'");
  }

  template_parser parser;

  auto part = parser.parse_until_command(cursor, "endfor");

  return std::make_shared<loop_template_part>(part, list_name, elem_name);
}

}
}
}
