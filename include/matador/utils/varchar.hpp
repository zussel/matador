//
// Created by sascha on 17.09.19.
//

#ifndef MATADOR_VARCHAR_HPP
#define MATADOR_VARCHAR_HPP

#include "is_varchar.hpp"

#include <type_traits>
#include <string>

namespace matador {

struct varchar_base {};

template<int SIZE, class T = std::string, class Enabled = void>
struct varchar;

template<int SIZE, class T>
struct varchar<SIZE, T, typename std::enable_if<
  is_varchar<T, SIZE>::value &&
  std::is_array<T>::value>
::type
> : public varchar_base
{
  typedef typename std::decay<T>::type value_type;

  explicit varchar(const value_type *value) {
    size_t s = strlen(value);
    s = s < size ? s : size;
    strncpy(value, value, s);
    value[s] = '\0';
  }
  int size = SIZE;
  value_type value[SIZE];
};

template<int SIZE, class T>
struct varchar<SIZE, T, typename std::enable_if<
  is_varchar<T, SIZE>::value &&
  !std::is_array<T>::value>
::type
> : public varchar_base
{
  typedef T value_type;

  explicit varchar(const value_type &val)
    : value(val)
  {}

  int size = SIZE;
  value_type value;
};
}
#endif //MATADOR_VARCHAR_HPP
