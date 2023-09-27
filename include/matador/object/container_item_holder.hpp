#ifndef MATADOR_CONTAINER_ITEM_HOLDER_HPP
#define MATADOR_CONTAINER_ITEM_HOLDER_HPP

#include "matador/object/value_type_traits.hpp"
#include "matador/object/basic_container_item_holder.hpp"

namespace matador {

/// @cond MATADOR_DEV

namespace detail
{
  struct basic_relation_endpoint;
}

class object_proxy;

template < class Type >
class container_item_holder : public basic_container_item_holder
{
public:
  using value_type = typename value_type_traits<Type>::value_type;

  container_item_holder() = default;

  container_item_holder(const container_item_holder &x)
  : basic_container_item_holder(x)
  , value_(x.value_)
  {}

  container_item_holder& operator=(const container_item_holder &x)
  {
    basic_container_item_holder::operator=(x);
    value_ = x.value_;
    return *this;
  }

  container_item_holder(container_item_holder &&x) noexcept
    : basic_container_item_holder(std::move(x))
  {
    value_ = std::move(x.value_);
  }

  container_item_holder& operator=(container_item_holder &&x) noexcept
  {
    if (this != &x) {
      basic_container_item_holder::operator=(std::move(x));
      value_ = std::move(x.value_);
    }
    return *this;
  }

  container_item_holder(const value_type &val, object_proxy *item_proxy)
  : basic_container_item_holder(item_proxy)
  , value_(val)
  {}

  container_item_holder(object_proxy *p, object_proxy *item_proxy)
  : basic_container_item_holder(item_proxy)
  , value_(p)
  {}

  friend bool operator==(const container_item_holder &a, const container_item_holder &b)
  {
    return a.value_ == b.value_;
  }

  friend bool operator!=(const container_item_holder &a, const container_item_holder &b)
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

  void clear() {}

private:
  friend struct detail::basic_relation_endpoint;

  value_type value_;
};

/// @endcond

}
#endif //MATADOR_CONTAINER_ITEM_HOLDER_HPP
