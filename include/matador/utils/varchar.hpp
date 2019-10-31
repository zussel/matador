//
// Created by sascha on 17.09.19.
//

#ifndef MATADOR_VARCHAR_HPP
#define MATADOR_VARCHAR_HPP

#include "is_varchar.hpp"

#include <type_traits>
#include <string>
#include <cstring>
#include <utility>

namespace matador {

struct varchar_base {};

template<int SIZE, class T = std::string>
struct varchar;

template<int SIZE>
struct varchar<SIZE, std::string> : public varchar_base
{
  typedef std::string value_type;

  varchar() = default;
  explicit varchar(value_type val)
  : value(std::move(val))
  {}

  void assign(const std::string &val)
  {
    value = val;
  }

  void assign(const char *val)
  {
    value.assign(val);
  }

  int size = SIZE;
  value_type value;
};

template<int SIZE>
struct varchar<SIZE, char> : public varchar_base
{
  typedef char value_type;

  varchar() = default;
  explicit varchar(const value_type *val) {
    assign(val);
  }

  void assign(const std::string &val)
  {
    assign(val.c_str());
  }

  void assign(const char *val)
  {
    auto len = strlen(val);
    len = len > size ? size : len;
#ifdef _MSC_VER
    strncpy_s(value, size, val, len);
#else
    strncpy(value, val, len);
#endif
    value[len] = '\0';
  }

  int size = SIZE;
  value_type value[SIZE] = {};
};

template<int SIZE>
struct varchar<SIZE, const char*> : public varchar_base
{
  typedef std::string value_type;

  varchar() = default;
  explicit varchar(value_type val)
    : value(std::move(val))
  {}

  void assign(const std::string &val)
  {
    value = val;
  }

  void assign(const char *val)
  {
    value.assign(val);
  }

  int size = SIZE;
  value_type value;
};

}
#endif //MATADOR_VARCHAR_HPP
