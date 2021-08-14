#ifndef MATADOR_IS_BUILTIN_HPP
#define MATADOR_IS_BUILTIN_HPP

#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

#include <type_traits>
#include <string>

namespace matador {

/// @cond MATADOR_DEV

struct varchar_base;

template <typename T, typename Enabled = void>
struct is_builtin {
  static const bool value = false;
};

template <typename T>
struct is_builtin< T,
  typename std::enable_if<
    std::is_scalar<T>::value ||
    std::is_convertible<T*, varchar_base*>::value ||
    std::is_same<T, std::string>::value ||
    std::is_same<T, matador::date>::value ||
    std::is_same<T, matador::time>::value>::type
>
{
  static const bool value = true;
};

/// @endcond

}

#endif //MATADOR_IS_BUILTIN_HPP
