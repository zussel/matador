//
// Created by sascha on 01.12.19.
//

#include "matador/utils/json.hpp"

#include <sstream>
#include <iostream>

namespace matador {

json::json() : value_(), type(e_null) {}

json::json(bool b)
  : value_(b), type(e_boolean)
{}

json::json(std::initializer_list<json> l)
  : type(e_array)
{
  bool is_obj = std::all_of(l.begin(), l.end(), [](const json &j) {
    return j.is_array() && j.size() == 2 && j[0].is_string();
  });
  if (is_obj) {
    auto *obj = new object_type;
    for (auto &i : l) {
      obj->insert(std::make_pair(i.value_.array->at(0).as<std::string>(), i[1]));
    }
    clear();
    type = e_object;
    value_.object = obj;
  } else {
    value_.array = new array_type(l);
  }
}

json::json(std::nullptr_t) : type(e_null)
{

}

json::json(json::json_type t) : value_(), type(t)
{
  switch (type) {
    case e_string:
      value_.str = new std::string();
      break;
    case e_object:
      value_.object = new object_type;
      break;
    case e_array:
      value_.array = new array_type;
      break;
    default:
      break;
  }
}

json::~json()
{
  clear();
}

json json::object()
{
  return json(e_object);
}

json json::array()
{
  return json(e_array);
}

json::json(const json &x)
{
  copy_from(x);
}

json::json(json &&x) noexcept
{
  value_ = x.value_;
  type = x.type;
  x.type = e_null;
}

json &json::operator=(json &&x) noexcept
{
  clear();
  value_ = x.value_;
  type = x.type;
  x.type = e_null;
  return *this;
}

json &json::operator=(const json &x)
{
  clear();
  copy_from(x);
  return *this;
}

std::ostream &operator<<(std::ostream &out, json &val)
{
  val.dump(out);
  return out;
}

void json::dump(std::ostream &out) const
{
  switch (type) {
    case e_object:
      out << "{";
      join(*value_.object, out, ", ");
      out << "}";
      break;
    case e_array:
      out << "[";
      join(*value_.array, out, ", ");
      out << "]";
      break;
    case e_null:
      out << "null";
      break;
    case e_integer:
      out << value_.integer;
      break;
    case e_real:
      out << value_.real;
      break;
    case e_string:
      out << "\"" << *value_.str << "\"";
      break;
    case e_boolean:
      out << std::boolalpha << value_.boolean;
      break;
    default:
      break;
  }
}

void json::copy_from(const json &x)
{
  type = x.type;
  switch (x.type) {
    case e_object:
      value_.object = new object_type(*x.value_.object);
      break;
    case e_array:
      value_.array = new array_type(*x.value_.array);
      break;
    case e_string:
      value_.str = new std::string(*x.value_.str);
      break;
    default:
      value_ = x.value_;
  }
}

json &json::operator[](const char *key)
{
  return this->operator[](std::string(key));
}

json &json::operator[](std::size_t i)
{
  if (type != e_array) {
    return *this;
  }
  if (i >= value_.array->size()) {
    throw std::logic_error("index out of bounce");
  }
  return value_.array->at(i);
}

void json::reset(bool b)
{
  if (type != e_boolean) {
    clear();
    type = e_boolean;
  }
  value_.boolean = b;
}

std::size_t json::size() const
{
  switch (type) {
    case e_array:
      return value_.array->size();
    case e_object:
      return value_.object->size();
    default:
      throw std::logic_error("type doesn't have size()");
  }
}

bool json::empty() const
{
  return false;
}

json &json::back()
{
  if (type == e_array) {
    return value_.array->back();
  } else {
    throw std::logic_error("type doesn't have back()");
  }
}

const json &json::back() const
{
  if (type == e_array) {
    return value_.array->back();
  } else {
    throw std::logic_error("type doesn't have back()");
  }
}

void json::erase(const std::string &key)
{
  if (type != e_object) {
    return;
  }
  value_.object->erase(key);
}

void json::erase(std::size_t i)
{
  if (type != e_array) {
    return;
  }
  if (i >= value_.array->size()) {
    throw std::out_of_range("index is out of range");
  }
  value_.array->erase(value_.array->begin()+i);
}

bool json::is_number() const
{
  return type == e_integer || type == e_real;
}

bool json::is_boolean() const
{
  return type == e_boolean;
}

bool json::is_string() const
{
  return type == e_string;
}

bool json::is_array() const
{
  return type == e_array;
}

bool json::is_object() const
{
  return type == e_object;
}

bool json::is_null() const
{
  return type == e_null;
}

void json::throw_on_wrong_type(json::json_type t) const
{
  if (type != t) {
    throw std::logic_error("wrong type; couldn't cast");
  }
}

json::iterator json::begin()
{
  return iterator(this, true);
}

json::iterator json::end()
{
  return iterator(this, false);
}

std::string to_string(const matador::json &j)
{
  std::stringstream out;
  j.dump(out);
  return out.str();
}

}