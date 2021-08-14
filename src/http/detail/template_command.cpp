#include "matador/http/detail/template_command.hpp"
#include "matador/http/detail/template_parser.hpp"

#include "matador/utils/string_cursor.hpp"
#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

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

std::shared_ptr<template_part> foreach_command::parse(string_cursor &cursor)
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
void add_expression(
  std::list<std::pair<std::shared_ptr<template_expression>, template_part_ptr>> &expression_list,
  std::unique_ptr<detail::multi_template_part> &part,
  const std::shared_ptr<template_expression>& expr
)
{
  std::shared_ptr<multi_template_part> condition_part;
  move_parts(condition_part, part);
  expression_list.emplace_back(expr, condition_part);
}

std::shared_ptr<template_part> if_else_command::parse(string_cursor &cursor)
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
      add_expression(expression_list, part, expr);
      has_else = true;
      return template_parser::INTERMEDIATE;
    } else if (cmd == "elif") {
      add_expression(expression_list, part, expr);
      expr = detail::parse_expression(cursor);
      return template_parser::INTERMEDIATE;
    } else if (cmd == "endif") {
      if (!has_else) {
        add_expression(expression_list, part, expr);
      }
      return template_parser::FINISHED;
    } else {
      return template_parser::NEXT_COMMAND;
    }
  });

  return std::make_shared<if_template_part>(expression_list, part);
}

std::shared_ptr<template_part> include_command::parse(string_cursor &cursor)
{
  char c = cursor.skip_whitespace();
  if (c == '"') {
    // read real path of file to include
    cursor.next_char();
    auto filepath = parse_filepath(cursor);

    c = cursor.current_char();

    if (c != '"') {
      // error
    }
    cursor.next_char();

    if (!os::exists(filepath)) {
      throw std::logic_error("file " + filepath + " doesn't exists");
    }
    file template_file(filepath, "r");
    std::string content = read_as_text(template_file);
    template_file.close();

    template_parser parser;
    string_cursor content_cursor(content.c_str());

    cursor.skip_whitespace();
    return parser.parse(content_cursor, [](const std::string&, std::unique_ptr<detail::multi_template_part>&) { return detail::template_parser::NEXT_COMMAND; });
  } else {
    // read variable name which evaluates to path name from json data
    auto var = parse_token(cursor);
  }

  if (!detail::parse_end_of_command_tag(cursor)) {
    throw std::logic_error("couldn't read expected command end tag '%}'");
  }

  // read and parse template file
  return std::shared_ptr<template_part>();
}

}
}
}
