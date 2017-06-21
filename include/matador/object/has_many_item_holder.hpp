//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_HAS_MANY_ITEM_HOLDER_HPP

#include "matador/utils/is_builtin.hpp"

namespace matador {

class object_proxy;
template < class T, object_holder_type OHT >
class object_pointer;
template < class T, class Enable = void >
class has_many_item_holder;

namespace detail {
struct basic_relation_endpoint;
}

template < class T >
class has_many_item_holder<T, typename std::enable_if<!is_builtin<T>::value>::type>
{
public:
  has_many_item_holder() {}
  has_many_item_holder(const object_ptr<T> &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  template < class V >
  has_many_item_holder(const object_ptr<T> &val, const object_ptr<V> &owner)
    : value_(val)
    , has_many_to_many_item_poxy_(owner.proxy_)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : value_(x.value_)
    , has_many_to_many_item_poxy_(x.has_many_to_many_item_poxy_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    return *this;
  }

  has_many_item_holder(has_many_item_holder &&x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    x.value_ = nullptr;
    x.has_many_to_many_item_poxy_ = nullptr;
  }

  has_many_item_holder& operator=(has_many_item_holder &&x)
  {
    if (this != &x) {
      value_ = x.value_;
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
      x.value_ = nullptr;
      x.has_many_to_many_item_poxy_ = nullptr;
    }
    return *this;
  }

  friend bool operator==(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ != b.value_;
  }

  object_ptr<T> value() const
  {
    return value_;
  }

  object_proxy* item_proxy() const
  {
    return has_many_to_many_item_poxy_;
  }

private:
  friend class detail::basic_relation_endpoint;

  object_ptr<T> value_;
  object_proxy *has_many_to_many_item_poxy_ = nullptr;
};


template < class T >
class has_many_item_holder<T, typename std::enable_if<is_builtin<T>::value>::type>
{
public:
  has_many_item_holder() {}
  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  template < class V >
  has_many_item_holder(const T &val, const object_ptr<V> &owner)
    : value_(val)
    , has_many_to_many_item_poxy_(owner.proxy_)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : value_(x.value_)
    , has_many_to_many_item_poxy_(x.has_many_to_many_item_poxy_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    return *this;
  }

  has_many_item_holder(has_many_item_holder &&x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    x.value_ = nullptr;
    x.has_many_to_many_item_poxy_ = nullptr;
  }

  has_many_item_holder& operator=(has_many_item_holder &&x)
  {
    if (this != &x) {
      value_ = x.value_;
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
      x.value_ = nullptr;
      x.has_many_to_many_item_poxy_ = nullptr;
    }
    return *this;
  }

  friend bool operator==(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ != b.value_;
  }

  T value() const
  {
    return value_;
  }
private:
  friend class detail::basic_relation_endpoint;

  T value_;
  object_proxy *has_many_to_many_item_poxy_;
};

}
#endif //MATADOR_HAS_MANY_ITEM_HOLDER_HPP
