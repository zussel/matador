#include "matador/http/detail/template_state.hpp"
#include "matador/http/detail/template_parser.hpp"

#include "matador/utils/string_cursor.hpp"

#include <memory>
#include <stdexcept>

namespace matador {
namespace http {
namespace detail {

void move_parts(std::shared_ptr<multi_template_part> &to_parts, std::unique_ptr<detail::multi_template_part> &from_part)
{
  to_parts.reset(from_part.release());
  from_part = std::make_unique<multi_template_part>();
}

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

  auto part = parser.parse(cursor, [this](const std::string &cmd, std::unique_ptr<detail::multi_template_part> &part) {
    if (cmd == "empty") {
      move_parts(loop_part_, part);
      return template_parser::INTERMEDIATE;
    } else if (cmd == "endfor") {
      // if loop part already set, current part is empty part
      // else we can set current part to loop part
      if (!loop_part_) {
        move_parts(loop_part_, part);
      }
      return template_parser::FINISHED;
    } else {
      return template_parser::NEXT_COMMAND;
    }
  });

  return std::make_shared<loop_template_part>(part, loop_part_, list_name, elem_name);
}

/*
 * if else state
 */
std::shared_ptr<template_part> if_else_state::parse(string_cursor &cursor)
{
  // parse until closing tag 'else' or 'elif' is read
  cursor.skip_whitespace();

  auto expr = detail::parse_expression(cursor);

  if (!detail::parse_end_of_command_tag(cursor)) {
    throw std::logic_error("couldn't read expected command end tag '%}'");
  }

  std::list<std::pair<std::shared_ptr<template_expression>, template_part_ptr>> expression_list;
  template_parser parser;
  bool has_else = false;
  auto part = parser.parse(cursor, [&cursor, &has_else, &expression_list, &expr](const std::string &cmd, std::unique_ptr<detail::multi_template_part> &part) {
    if (cmd == "else") {
      std::shared_ptr<multi_template_part> condition_part;
      move_parts(condition_part, part);
      expression_list.push_back(std::make_pair(expr, condition_part));
      has_else = true;
      return template_parser::INTERMEDIATE;
    } else if (cmd == "elif") {
      std::shared_ptr<multi_template_part> condition_part;
      move_parts(condition_part, part);
      expression_list.push_back(std::make_pair(expr, condition_part));
      expr = detail::parse_expression(cursor);
      return template_parser::INTERMEDIATE;
    } else if (cmd == "endif") {
      if (!has_else) {
        std::shared_ptr<multi_template_part> condition_part;
        move_parts(condition_part, part);
        expression_list.push_back(std::make_pair(expr, condition_part));
      }
      return template_parser::FINISHED;
    } else {
      return template_parser::NEXT_COMMAND;
    }
  });

  return std::make_shared<if_template_part>(expression_list, part);
}
}
}
}
