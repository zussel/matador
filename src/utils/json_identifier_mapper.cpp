//
// Created by sascha on 12.01.20.
//

#include "matador/utils/json_identifier_mapper.hpp"
#include "matador/utils/basic_identifier.hpp"

#include <cstring>

namespace matador {

void json_identifier_mapper::set_identifier_value(basic_identifier &id, const json &val)
{
  value_ = val;
  id.serialize("", *this);
}

template < class T >
void assign_integer(const json &value, T &x)
{
  if (value.is_integer()) {
    x = value.as<T>();
  }
}

void json_identifier_mapper::serialize(const char *, char &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, short &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, int &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, long &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, long long &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, unsigned char &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, unsigned short &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, unsigned int &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, unsigned long &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, unsigned long long &x)
{
  assign_integer(value_, x);
}

void json_identifier_mapper::serialize(const char *, char *x, size_t s)
{
  if (!value_.is_string()) {
    return;
  }
#ifdef _MSC_VER
  strncpy_s(x, s, value_.as<std::string>().c_str(), s);
#else
  strncpy(x, value_.as<std::string>().c_str(), s);
#endif
  x[value_.as<std::string>().size()] = '\0';
}

void json_identifier_mapper::serialize(const char *, std::string &x, size_t)
{
  if (!value_.is_string()) {
    return;
  }
  x = value_.as<std::string>();
}
}