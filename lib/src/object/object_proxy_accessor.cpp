//
// Created by sascha on 9/2/16.
//

#include <object/object_holder.hpp>
#include "object/object_proxy_accessor.hpp"

namespace oos {

namespace detail {

oos::object_proxy *object_proxy_accessor::proxy(const object_holder &holder) const
{
  return holder.proxy_;
}

}

}