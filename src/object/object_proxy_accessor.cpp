//
// Created by sascha on 9/2/16.
//

#include "matador/object/object_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

namespace matador {

namespace detail {

matador::object_proxy *object_proxy_accessor::proxy(const object_holder &holder) const
{
  return holder.proxy_;
}

}

}