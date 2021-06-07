#ifndef MATADOR_ENUM_CLASS_HASH_HPP
#define MATADOR_ENUM_CLASS_HASH_HPP

#include <cstddef>

namespace matador {
namespace http {
namespace detail {
/// @cond MATADOR_DEV
struct enum_class_hash
{
  template<typename T>
  std::size_t operator()(T t) const
  {
    return static_cast<std::size_t>(t);
  }
};
/// @endcond
}
}
}
#endif //MATADOR_ENUM_CLASS_HASH_HPP
