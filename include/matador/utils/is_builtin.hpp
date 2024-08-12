#ifndef MATADOR_IS_BUILTIN_HPP
#define MATADOR_IS_BUILTIN_HPP

#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

#include <type_traits>
#include <string>

namespace matador {

/// @cond MATADOR_DEV

template <typename Type, typename Enabled = void>
struct is_builtin {
  static constexpr bool value = false;
};

template <typename Type>
struct is_builtin< Type,
  std::enable_if_t<
    std::is_scalar_v<Type> ||
    std::is_same_v<Type, std::string> ||
    std::is_same_v<Type, const char*> ||
    std::is_same_v<Type, char*> ||
    std::is_same_v<Type, date> ||
    std::is_same_v<Type, time> ||
    std::is_same_v<Type, utils::blob>>
>
{
  static constexpr bool value = true;
};

/// @endcond

}

#endif //MATADOR_IS_BUILTIN_HPP
