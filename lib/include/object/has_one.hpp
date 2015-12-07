//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_ONE_HPP
#define OOS_HAS_ONE_HPP

#include "object/object_proxy.hpp"
#include "object_proxy.hpp"

namespace oos {

template < class T >
class has_one
{
public:
  has_one() {}
  has_one(T *o)
    : proxy_(new object_proxy(o))
  {}
  has_one(object_proxy *proxy)
    : proxy_(proxy)
  {}

  T* operator->()
  {
    return static_cast<T*>(proxy_->obj());
  }

private:
  object_proxy *proxy_ = nullptr;
  bool is_reference_ = false;
  unsigned long oid_ = 0;
};

}

#endif //OOS_HAS_ONE_HPP
