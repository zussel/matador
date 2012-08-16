#include "json/json_parser.hpp"
#include "json/json_object.hpp"
#include "json/json_array.hpp"

#include <sstream>

namespace oos {

json_parser::json_parser()
  : generic_json_parser<json_parser>(this)
{}

json_parser::~json_parser()
{}

json_value json_parser::parse(std::istream &in)
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

json_value json_parser::parse(std::string &str)
{
  std::istringstream in(str);
  return parse(in);
}

json_value json_parser::parse(const char *str)
{
  std::istringstream in(str);
  return parse(in);
}

void json_parser::on_begin_object()
{
  json_value value(new json_object);
  if (state_stack_.empty()) {
    value_ = value;
  } else {
    if (state_stack_.top().first) {
      state_stack_.top().second[key_] = value;
    } else {
      state_stack_.top().second.push_back(value);
    }
  }
  state_stack_.push(std::make_pair(true, value));
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
  json_value value(new json_array);
  if (state_stack_.empty()) {
    value_ = value;
  } else {
    if (state_stack_.top().first) {
      state_stack_.top().second[key_] = value;
    } else {
      state_stack_.top().second.push_back(value);
    }
  }
  state_stack_.push(std::make_pair(false, value));
}

void json_parser::on_end_array()
{
  state_stack_.pop();
}

void json_parser::on_string(const std::string &value)
{
  if (state_stack_.top().first) {
    state_stack_.top().second[key_] = value;
  } else {
    state_stack_.top().second.push_back(value);
  }
}

void json_parser::on_number(double value)
{
  if (state_stack_.top().first) {
    state_stack_.top().second[key_] = value;
  } else {
    state_stack_.top().second.push_back(value);
  }
}

void json_parser::on_bool(bool value)
{
  if (state_stack_.top().first) {
    state_stack_.top().second[key_] = value;
  } else {
    state_stack_.top().second.push_back(value);
  }
}

void json_parser::on_null()
{
  if (state_stack_.top().first) {
    state_stack_.top().second[key_] = json_value();
  } else {
    state_stack_.top().second.push_back(json_value());
  }
}

}