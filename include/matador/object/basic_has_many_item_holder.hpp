//
// Created by sascha on 26.08.17.
//

#ifndef MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
#define MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP

namespace matador {

class object_proxy;

namespace detail {
struct basic_relation_endpoint;
}

class basic_has_many_item_holder
{
protected:
  basic_has_many_item_holder() = default;

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
  friend struct detail::basic_relation_endpoint;
  object_proxy *has_many_to_many_item_poxy_ = nullptr;
};

}

#endif //MATADOR_BASIC_HAS_MANY_ITEM_HOLDER_HPP
