//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_HAS_MANY_ITEM_HOLDER_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"

namespace matador {

class object_proxy;

template < class T, class Enable = void >
class has_many_item_holder;

template < class T >
class has_many_item_holder<T, typename std::enable_if<!is_builtin<T>::value>::type>
{
public:
  has_many_item_holder(const object_ptr<T> &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  object_ptr<T> value() const
  {
    return value_;
  }
private:
  object_ptr<T> value_;
  object_proxy *has_many_to_many_item_poxy_;
};

template < class T >
class has_many_item_holder<T, typename std::enable_if<is_builtin<T>::value>::type>
{
public:
  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  T value() const
  {
    return value_;
  }
private:
  T value_;
  object_proxy *has_many_to_many_item_poxy_;
};

}
#endif //MATADOR_HAS_MANY_ITEM_HOLDER_HPP
