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
  int size = SIZE;
  typedef typename std::decay<T>::type value_type;

  const char *type_name()
  {
    return typeid(value_type).name();
  }
};

template<int SIZE, class T>
struct varchar<SIZE, T, typename std::enable_if<
  is_varchar<T, SIZE>::value &&
  !std::is_array<T>::value>
::type
> : public varchar_base
{
  int size = SIZE;
  typedef T value_type;

  const char *type_name()
  {
    return typeid(value_type).name();
  }
};
}
#endif //MATADOR_VARCHAR_HPP
