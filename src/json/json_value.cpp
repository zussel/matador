#include "json/json_value.hpp"
#include "json/json_object.hpp"
#include "json/json_string.hpp"
#include "json/json_bool.hpp"
#include "json/json_null.hpp"
#include "json/json_number.hpp"
#include "json/json_array.hpp"

#include <iostream>

namespace oos {

json_value::json_value()
  : ptr(new type<json_object>(json_object()))
{}

json_value::json_value(const json_value &x)
  : ptr(x.ptr)
{
}

json_value::json_value(const json_object &x)
  : ptr(new type<json_object>(x))
{
}

json_value::json_value(const json_bool &x)
  : ptr(new type<json_bool>(x))
{
}

json_value::json_value(const json_string &x)
  : ptr(new type<json_string>(x))
{
}

json_value::json_value(const json_array &x)
  : ptr(new type<json_array>(x))
{}

json_value::json_value(const json_number &x)
  : ptr(new type<json_number>(x))
{}

json_value::json_value(const json_null &x)
  : ptr(new type<json_null>(x))
{}

json_value::json_value(const std::string &x)
  : ptr(new type<json_string>(x))
{
}

json_value::json_value(const char *x)
  : ptr(new type<json_string>(x))
{
}

json_value& json_value::operator=(const json_value &x)
{
  ptr = x.ptr;
  return *this;
}

json_value& json_value::operator=(double x)
{
  ptr.reset(new type<json_number>(x));
  return *this;
}

json_value::~json_value()
{}

json_value& json_value::operator[](const std::string &key)
{
  return ptr->operator [](key);
}

json_value& json_value::operator[](size_t index)
{
  return ptr->operator [](index);
}

const json_value& json_value::operator[](size_t index) const
{
  return ptr->operator [](index);
}

void json_value::push_back(const json_value &x)
{
  ptr->push_back(x);
}

json_value json_value::create(std::istream &in)
{
  // eat whitespace
  in >> std::ws;

  char c = in.peek();

  if (!in.good()) {
    throw std::logic_error("invalid stream");
  }

  switch (c) {
    case '{':
      return json_value(json_object());
    case '[':
      return json_value(json_array());
    case '"':
      return json_value(json_string());
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return json_value(json_number());
    case 't':
    case 'f':
      return json_value(json_bool());
    case 'n':
      //return new json_null;
      throw std::logic_error("json_null not yet implemented");
    default:
      throw std::logic_error("unknown json type");
  }
}

std::istream& operator>>(std::istream &str, json_value &value)
{
  value.ptr->parse(str);
  return str;
}

std::ostream& operator<<(std::ostream &str, const json_value &value)
{
  value.ptr->print(str);
  return str;
}

}
