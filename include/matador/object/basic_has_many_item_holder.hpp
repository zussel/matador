//
// Created by sascha on 26.08.17.
//

#ifndef MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP

namespace matador {

/// @cond MATADOR_DEV

namespace detail {
struct basic_relation_endpoint;
class object_inserter;
}

class abstract_has_many;
class object_proxy;

class basic_has_many_item_holder
{
protected:
  basic_has_many_item_holder() = default;
  explicit basic_has_many_item_holder(object_proxy *item_proxy)
    : has_many_to_many_item_poxy_(item_proxy)
  {}

  basic_has_many_item_holder(const basic_has_many_item_holder &x) = default;
  basic_has_many_item_holder(basic_has_many_item_holder &&x) noexcept = default;
  basic_has_many_item_holder& operator=(basic_has_many_item_holder &&x) noexcept = default;
  basic_has_many_item_holder& operator=(const basic_has_many_item_holder &x) noexcept = default;

public:
  bool is_inserted() const { return is_inserted_; }

  object_proxy* item_proxy() const { return has_many_to_many_item_poxy_; }

protected:
  friend struct detail::basic_relation_endpoint;
  friend class detail::object_inserter;
  friend class abstract_has_many;

  bool is_inserted_ = false;
  object_proxy *has_many_to_many_item_poxy_ = nullptr;
};

/// @endcond

}

#endif //MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
