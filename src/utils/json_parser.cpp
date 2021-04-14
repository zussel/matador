//
// Created by sascha on 22.12.19.
//

#include "matador/utils/json_parser.hpp"

#include <sstream>

namespace matador
{

//json json_parser::parse(std::istream &in)
//{
//  // clear stack
//  while (!state_stack_.empty()) {
//    state_stack_.pop();
//  }
//
//  // call parser
////  parse_json(in);
//
//  // return json vale
//  return value_;
//}

json json_parser::parse(const char *str)
{
  while (!state_stack_.empty()) {
    state_stack_.pop();
  }

  parse_json(str);

  return value_;
//  std::istringstream in(str);
//  return parse(in);
}

json json_parser::parse(const std::string &str)
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
  on_value(value);
}

void json_parser::on_real(double value)
{
  on_value(value);
}

void json_parser::on_integer(long long value)
{
  on_value(value);
}

void json_parser::on_bool(bool value)
{
  on_value(value);
}

void json_parser::on_null()
{
  on_value(json());
}

void json_parser::on_value(const json &value)
{
  if (state_stack_.empty()) {
    value_ = value;
  } else if (state_stack_.top()->is_object()) {
    state_stack_.top()->operator[](key_) = value;
  } else if (state_stack_.top()->is_array()) {
    state_stack_.top()->push_back(value);
  } else {
    throw std::logic_error("invalid json error");
  }
}

}