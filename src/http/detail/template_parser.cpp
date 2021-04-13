#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_expression.hpp"

#include "matador/utils/string.hpp"

#include <set>

namespace matador {
namespace http {
namespace detail {

std::string parse_token(string_cursor &cursor)
{
  std::string token;
  char c = cursor.skip_whitespace();

  while(!is_eos(c)) {
    if (isalnum(c) || c == '.') {
      token.push_back(c);
    } else {
      break;
    }
    c = cursor.next_char();
  }

  cursor.skip_whitespace();
  return token;
}

bool parse_end_of_command_tag(string_cursor &cursor)
{
  if (cursor.skip_whitespace() != '%') {
    return false;
  } else if (cursor.next_char() != '}') {
    return false;
  } else {
    cursor.next_char();
  }
  return true;
}

std::shared_ptr<template_expression> parse_expression(string_cursor &cursor)
{
  static std::set<std::string> operators { "==", "!=", "<", "<=", ">", ">=" };

  auto token = parse_token(cursor);

  char c = cursor.current_char();
  if (c == '%') {
    // end of expression
    return std::make_shared<json_value_expression>(token);
  } else {
    // parse operator
    std::string op = parse_operator(cursor);

    if (operators.find(op) == operators.end()) {
      throw std::logic_error("invalid operator " + op);
    }

    // parse operand
    // if operand is in double quotes it is treated as a string
    //
    return std::make_shared<json_value_expression>(token);

  }
}

bool isop(char c)
{
  return c == '=' || c == '!' || c == '<' || c == '>';
}

std::string parse_operator(string_cursor &cursor)
{
  // valid operators:
  // ==,!=,<,<=,>,>=
  std::string op;
  char c = cursor.current_char();
  while (!isspace(c) && !is_eos(c)) {
    if (isop(c)) {
      op += c;
    } else {
      throw std::logic_error("character is not operator " + std::string(c, 1));
    }
    c = cursor.next_char();
  }

  return op;
}

}
}
}