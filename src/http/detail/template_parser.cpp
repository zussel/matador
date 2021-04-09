#include "matador/http/detail/template_parser.hpp"
#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_state_factory.hpp"

#include "matador/utils/string_cursor.hpp"
#include "matador/utils/string.hpp"

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

std::shared_ptr<template_part> template_parser::parse(string_cursor &cursor)
{
  return parse_until_command(cursor, "");
}

std::shared_ptr<template_part> template_parser::parse_until_command(string_cursor &cursor, const std::string &stop_cmd)
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
      if (!stop_cmd.empty() && cmd == stop_cmd) {
        return std::shared_ptr<template_part>(parts_.release());;
      }
      handle_command(cursor, cmd);
      c = cursor.current_char();
      continue;
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

void template_parser::handle_command(string_cursor &cursor, const std::string &cmd)
{
  auto cmdptr = detail::template_state_factory::instance().produce(cmd);

  parts_->push_back(cmdptr->parse(cursor));
}
}
}
}