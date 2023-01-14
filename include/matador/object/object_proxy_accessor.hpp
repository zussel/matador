#ifndef OOS_OBJECT_PROXY_ACCESSOR_HPP
#define OOS_OBJECT_PROXY_ACCESSOR_HPP

#include "matador/object/export.hpp"

namespace matador {

class object_proxy;
class object_holder;

namespace detail {

/// @cond MATADOR_DEV

class MATADOR_OBJECT_API object_proxy_accessor
{
public:
  matador::object_proxy* proxy(const object_holder &holder) const;
};

/// @endcond

}
}
#endif //OOS_OBJECT_PROXY_ACCESSOR_HPP
