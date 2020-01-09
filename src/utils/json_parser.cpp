//
// Created by sascha on 22.12.19.
//

#include "matador/utils/json_parser.hpp"

#include <sstream>

namespace matador
{
json_parser::json_parser()
  : generic_json_parser<json_parser>(this)
{}

json json_parser::parse(std::istream &in)
{
  /*
   * clear stack
   */
  while (!state_stack_.empty()) {
    state_stack_.pop();
  }

  /*
   * call parser
   */
  parse_json(in);

  /*
   * return value
   */
  return value_;
}

json json_parser::parse(const char *str)
{
  std::istringstream in(str);
  return parse(in);
}

json json_parser::parse(std::string &str)
{
  return parse(str.c_str());
}

void json_parser::on_begin_object()
{
  if (!state_stack_.empty()) {
    if (state_stack_.top()->is_object()) {
      state_stack_.top()->operator[](key_) = json::object();
      state_stack_.push(&state_stack_.top()->operator[](key_));
    } else {
      state_stack_.top()->push_back(json::object());
      state_stack_.push(&state_stack_.top()->back());
    }
  } else {
    value_ = json::object();
    state_stack_.push(&value_);
  }
}

void json_parser::on_object_key(const std::string &key)
{
  key_ = key;
}

void json_parser::on_end_object()
{
  state_stack_.pop();
}

void json_parser::on_begin_array()
{
  if (state_stack_.empty()) {
    value_ = json::array();
    state_stack_.push(&value_);
  } else {
    if (state_stack_.top()->is_object()) {
      state_stack_.top()->operator[](key_) = json::array();
      state_stack_.push(&state_stack_.top()->operator[](key_));
    } else {
      state_stack_.top()->push_back(json::array());
      state_stack_.push(&state_stack_.top()->back());
    }
  }
}

void json_parser::on_end_array()
{
  state_stack_.pop();
}

void json_parser::on_string(const std::string &value)
{
  if (state_stack_.top()->is_object()) {
    state_stack_.top()->operator[](key_) = value;
  } else {
    state_stack_.top()->push_back(value);
  }
}

void json_parser::on_number(generic_json_parser<json_parser>::real_t value)
{
  if (state_stack_.top()->is_object()) {
    if (value.is_real) {
      state_stack_.top()->operator[](key_) = value.real;
    } else {
      state_stack_.top()->operator[](key_) = value.integer;
    }
  } else {
    if (value.is_real) {
      state_stack_.top()->push_back(value.real);
    } else {
      state_stack_.top()->push_back(value.integer);
    }
  }
}

void json_parser::on_bool(bool value)
{
  if (state_stack_.top()->is_object()) {
    state_stack_.top()->operator[](key_) = value;
  } else {
    state_stack_.top()->push_back(value);
  }
}

void json_parser::on_null()
{
  if (state_stack_.top()->is_object()) {
    state_stack_.top()->operator[](key_) = json();
  } else {
    state_stack_.top()->push_back(json());
  }
}
}