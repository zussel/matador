//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_HAS_MANY_ITEM_HOLDER_HPP

#include "matador/utils/varchar.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/basic_has_many_item_holder.hpp"

namespace matador {

/// @cond MATADOR_DEV

namespace detail
{
  struct basic_relation_endpoint;
}

class object_proxy;

template < class T >
class has_many_item_holder : public basic_has_many_item_holder
{
public:
  typedef T object_type;
  typedef T value_type;

  has_many_item_holder() = default;

  has_many_item_holder(const T &val, object_proxy *item_proxy)
  : basic_has_many_item_holder(item_proxy)
  , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
  : basic_has_many_item_holder(x.has_many_to_many_item_poxy_)
  , value_(x.value_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    value_ = x.value_;
    return *this;
  }

  has_many_item_holder(object_proxy *, std::nullptr_t) {}

  has_many_item_holder(has_many_item_holder &&x) noexcept
    : basic_has_many_item_holder(std::move(x))
  {
    value_ = std::move(x.value_);
  }

  has_many_item_holder& operator=(has_many_item_holder &&x) noexcept
  {
    if (this != &x) {
      value_ = std::move(x.value_);
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
      x.has_many_to_many_item_poxy_ = nullptr;
    }
    return *this;
  }

  friend bool operator==(const has_many_item_holder &a, const has_many_item_holder &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const has_many_item_holder &a, const has_many_item_holder &b)
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

  void clear() {}

private:
  friend struct detail::basic_relation_endpoint;

  T value_;
};

template < class T >
class has_many_item_holder<object_ptr<T>> : public basic_has_many_item_holder
{
public:
  typedef object_ptr<T> object_type;
  typedef T value_type;

  has_many_item_holder() = default;

  has_many_item_holder(const object_type &val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(object_proxy *val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x.has_many_to_many_item_poxy_)
    , value_(x.value_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    return *this;
  }

  has_many_item_holder(has_many_item_holder &&x) noexcept
    : basic_has_many_item_holder(std::move(x))
  {
    value_ = std::move(x.value_);
  }

  has_many_item_holder& operator=(has_many_item_holder &&x) noexcept
  {
    if (this != &x) {
      basic_has_many_item_holder::operator=(std::move(x));
      value_ = std::move(x.value_);
    }
    return *this;
  }

  friend bool operator==(const has_many_item_holder &a, const has_many_item_holder &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const has_many_item_holder &a, const has_many_item_holder &b)
  {
    return a.value_ != b.value_;
  }

  const object_type& value() const
  {
    return value_;
  }

  object_type& value()
  {
    return value_;
  }

  void clear()
  {
    value_.reset(nullptr, value_.cascade());
  }
private:
  friend struct detail::basic_relation_endpoint;

  object_type value_;
};

template < int SIZE, class T >
class has_many_item_holder<varchar<SIZE, T>> : public basic_has_many_item_holder
{
public:
  typedef varchar<SIZE, T> object_type;
  typedef T value_type;

  has_many_item_holder() = default;

  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : basic_has_many_item_holder(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x.has_many_to_many_item_poxy_)
    , value_(x.value_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    value_ = x.value_;
    return *this;
  }

  has_many_item_holder(object_proxy *, std::nullptr_t) {}

  has_many_item_holder(has_many_item_holder &&x) noexcept
  {
    value_ = std::move(x.value_);

    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    x.has_many_to_many_item_poxy_ = nullptr;
  }

  has_many_item_holder& operator=(has_many_item_holder &&x) noexcept
  {
    if (this != &x) {
      value_ = std::move(x.value_);
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
      x.has_many_to_many_item_poxy_ = nullptr;
    }
    return *this;
  }

  friend bool operator==(const has_many_item_holder &a, const has_many_item_holder &b)
  {
    return a.value_.value == b.value_.value;
  }

  friend bool operator!=(const has_many_item_holder &a, const has_many_item_holder &b)
  {
    return a.value_.value != b.value_.value;
  }

  const T& value() const
  {
    return value_.value;
  }

  T& value()
  {
    return value_.value;
  }

  void clear() {}

private:
  friend struct detail::basic_relation_endpoint;

  object_type value_;
};

/// @endcond

}
#endif //MATADOR_HAS_MANY_ITEM_HOLDER_HPP
