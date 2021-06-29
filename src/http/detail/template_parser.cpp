#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_expression.hpp"
#include "matador/http/detail/template_filter_factory.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/json_parser.hpp"

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

std::string parse_filepath(string_cursor &cursor)
{
  std::string token;
  char c = cursor.skip_whitespace();

  while(!is_eos(c)) {
    if (isalnum(c) || c == '.' || c == '_' || c == '/') {
      token.push_back(c);
    } else {
      break;
    }
    c = cursor.next_char();
  }

  cursor.skip_whitespace();
  return token;
}

std::string parse_operand(string_cursor &cursor)
{
  std::string token;
  char c = cursor.skip_whitespace();

  while(!is_eos(c)) {
    if (isalnum(c) || c == '.' || c == '"') {
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

std::function<bool(const json&, const json&)> build_json_compare_function(const std::string &op);
std::function<bool(const json&)> build_compare_function(const std::string &op, const json &operand);

std::shared_ptr<template_filter> parse_filter(string_cursor &cursor)
{
  // extract filter name
  auto name = parse_token(cursor);

  auto filter = template_filter_factory::instance().produce(name);

  return filter;
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
    auto operand = parse_operand(cursor);
    if (isalpha(operand[0])) {
      return std::make_shared<json_json_compare_expression>(token, operand, build_json_compare_function(op));
    } else {
      json_parser parser;
      auto j = parser.parse(operand);
      return std::make_shared<json_compare_expression>(token, build_compare_function(op, j));
    }
  }
}

std::function<bool(const json&)> build_compare_function(const std::string &op, const json &operand)
{
  if (op == "==") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " == " << operand << "\n";
      return value == operand;
    };
  } else if (op == "!=") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " != " << operand << "\n";
      return value != operand;
    };
  } else if (op == "<") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " < " << operand << "\n";
      return value < operand;
    };
  } else if (op == "<=") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " <= " << operand << "\n";
      return value <= operand;
    };
  } else if (op == ">") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " > " << operand << "\n";
      return value > operand;
    };
  } else if (op == ">=") {
    return [operand](const json &value) {
      //std::cout << "comparing: " << value << " >= " << operand << "\n";
      return value >= operand;
    };
  } else {
    return [](const json &) {
      return false;
    };
  }
}

std::function<bool(const json&, const json&)> build_json_compare_function(const std::string &op)
{
  if (op == "==") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << left << " == " << right << "\n";
      return left == right;
    };
  } else if (op == "!=") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << value << " != " << right << "\n";
      return left != right;
    };
  } else if (op == "<") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << left << " < " << right << "\n";
      return left < right;
    };
  } else if (op == "<=") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << left << " <= " << right << "\n";
      return left <= right;
    };
  } else if (op == ">") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << left << " > " << right << "\n";
      return left > right;
    };
  } else if (op == ">=") {
    return [](const json &left, const json &right) {
      //std::cout << "comparing: " << left << " >= " << right << "\n";
      return left >= right;
    };
  } else {
    return [](const json &, const json &) {
      return false;
    };
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