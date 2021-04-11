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

  //on_empty_part_ = std::make_shared<multi_template_part>();
  auto part = parser.parse(cursor, [this](const std::string &cmd, std::unique_ptr<detail::multi_template_part> &part) {
    if (cmd == "empty") {

//      while (!part->parts().empty()) {
//        on_empty_part_->push_back(part->parts().front());
//        part->parts().pop_front();
//      }
      return false;
    } else {
      return cmd == "endfor";
    }
  });

  return std::make_shared<loop_template_part>(part, on_empty_part_, list_name, elem_name);
}

}
}
}
