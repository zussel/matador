//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_HAS_MANY_ITEM_HOLDER_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_holder_type.hpp"

namespace matador {

class object_proxy;
template < class T, object_holder_type OHT >
class object_pointer;

template < class T, class Enable = void >
class has_many_item_holder;

//template < class T >
//using object_ptr<T> = object_pointer<T, object_holder_type::OBJECT_PTR>;

namespace detail {
struct basic_relation_endpoint;
}

class basic_has_many_item_holder
{
protected:
  basic_has_many_item_holder() {}

  explicit basic_has_many_item_holder(object_proxy *item_proxy)
    : has_many_to_many_item_poxy_(item_proxy)
  {}

public:
  object_proxy* item_proxy() const
  {
    return has_many_to_many_item_poxy_;
  }

protected:
  void set_item_proxy(object_proxy *itemproxy)
  {
    has_many_to_many_item_poxy_ = itemproxy;
  }
private:
  friend class detail::basic_relation_endpoint;
  object_proxy *has_many_to_many_item_poxy_ = nullptr;
};

template < class T >
class has_many_item_holder<T, typename std::enable_if<!is_builtin<T>::value>::type> : public basic_has_many_item_holder
{
public:
  has_many_item_holder() {}

  has_many_item_holder(const object_ptr<T> &val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(object_proxy *val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  template < class V >
  has_many_item_holder(const object_ptr<T> &val, const object_ptr<V> &owner)
    : basic_has_many_item_holder(owner.proxy_)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x)
    , value_(x.value_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    value_ = x.value_;
    this->set_item_proxy(x.item_proxy());
    return *this;
  }

  has_many_item_holder(has_many_item_holder &&x)
  {
    value_ = x.value_;
    this->set_item_proxy(x.item_proxy());
    x.value_ = nullptr;
    x.set_item_proxy(nullptr);
  }

  has_many_item_holder& operator=(has_many_item_holder &&x)
  {
    if (this != &x) {
      value_ = x.value_;
      this->set_item_proxy(x.item_proxy());
      x.value_ = nullptr;
      x.set_item_proxy(nullptr);
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

  const object_ptr<T>& value() const
  {
    return value_;
  }

  object_ptr<T>& value()
  {
    return value_;
  }

private:
  object_ptr<T> value_;
};

template < class T >
class has_many_item_holder<T, typename std::enable_if<is_builtin<T>::value>::type> : public basic_has_many_item_holder
{
public:
  has_many_item_holder() {}

  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  template < class V >
  has_many_item_holder(const T &val, const object_ptr<V> &owner)
    : basic_has_many_item_holder(owner.proxy_)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x)
    , value_(x.value_)
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
//    x.value_ = nullptr;
    x.has_many_to_many_item_poxy_ = nullptr;
  }

  has_many_item_holder& operator=(has_many_item_holder &&x)
  {
    if (this != &x) {
      value_ = x.value_;
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
//      x.value_ = nullptr;
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

  const T& value() const
  {
    return value_;
  }

  T& value()
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
