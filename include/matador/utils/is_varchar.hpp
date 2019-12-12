//
// Created by sascha on 17.09.19.
//

#ifndef MATADOR_IS_VARCHAR_HPP
#define MATADOR_IS_VARCHAR_HPP

#include <type_traits>
#include <string>

namespace matador {

/// @cond MATADOR_DEV

template<class T>
struct is_c_str
: std::integral_constant<
bool,
std::is_same<char const *, typename std::decay<T>::type>::value ||
std::is_same<char *, typename std::decay<T>::type>::value
> {};

template <typename T, int SIZE = 0, typename Enabled = void>
struct is_varchar {
  static const bool value = false;
};

template <typename T, int SIZE>
struct is_varchar<T, SIZE, typename std::enable_if<
(std::is_same<T, std::string>::value ||
 is_c_str<T>::value
) &&
(SIZE > 0)>::type
>
{
  static const bool value = true;
};

/// @endcond

}

#endif //MATADOR_IS_VARCHAR_HPP
