//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_HAS_MANY_ITEM_HOLDER_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_holder_type.hpp"
#include "matador/object/basic_has_many_item_holder.hpp"

namespace matador {

namespace detail
{
  struct basic_relation_endpoint;
}

template < class T, object_holder_type OHT >
class object_pointer;

template < class T, class Enable = void >
class has_many_item_holder;

template < class T >
class has_many_item_holder<T, typename std::enable_if<!is_builtin<T>::value>::type> : public basic_has_many_item_holder
{
public:
  typedef object_pointer<T, object_holder_type::OBJECT_PTR> value_type;

  has_many_item_holder() = default;

  has_many_item_holder(const value_type &val, object_proxy *item_proxy)
    : has_many_to_many_item_poxy_(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(object_proxy *val, object_proxy *item_proxy)
    : has_many_to_many_item_poxy_(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x)
    , has_many_to_many_item_poxy_(x.has_many_to_many_item_poxy_)
    , value_(x.value_)
  {}

  has_many_item_holder& operator=(const has_many_item_holder &x)
  {
    value_ = x.value_;
    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    return *this;
  }

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

  friend bool operator==(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const has_many_item_holder<T> &a, const has_many_item_holder<T> &b)
  {
    return a.value_ != b.value_;
  }

  const value_type& value() const
  {
    return value_;
  }

  value_type& value()
  {
    return value_;
  }

  object_proxy* item_proxy() const
  {
    return has_many_to_many_item_poxy_;
  }

  void clear()
  {
    value_.reset(nullptr, value_.cascade());
  }
private:
  friend struct detail::basic_relation_endpoint;

  object_proxy *has_many_to_many_item_poxy_ = nullptr;
  value_type value_;
};

template < class T >
class has_many_item_holder<T, typename std::enable_if<is_builtin<T>::value>::type> : public basic_has_many_item_holder
{
public:
  typedef T value_type;

  has_many_item_holder() = default;
  ~has_many_item_holder() = default;

  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : has_many_to_many_item_poxy_(item_proxy)
    , value_(val)
  {}

  has_many_item_holder(const has_many_item_holder &x)
    : basic_has_many_item_holder(x)
    , has_many_to_many_item_poxy_(x.has_many_to_many_item_poxy_)
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
//    value_ = x.value_;

    has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
    x.has_many_to_many_item_poxy_ = nullptr;
  }

  has_many_item_holder& operator=(has_many_item_holder &&x) noexcept
  {
    if (this != &x) {
      value_ = std::move(x.value_);
      has_many_to_many_item_poxy_ = x.has_many_to_many_item_poxy_;
      x.has_many_to_many_item_poxy_ = nullptr;
//      basic_has_many_item_holder(x);
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

  object_proxy* item_proxy() const
  {
    return has_many_to_many_item_poxy_;
  }

  void clear() {}

private:
  friend struct detail::basic_relation_endpoint;

  object_proxy *has_many_to_many_item_poxy_ = nullptr;
  T value_;
};

}
#endif //MATADOR_HAS_MANY_ITEM_HOLDER_HPP
