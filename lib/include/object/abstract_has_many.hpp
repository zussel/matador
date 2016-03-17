//
// Created by sascha on 3/17/16.
//

#ifndef OOS_ABSTRACT_HAS_MANY_HPP
#define OOS_ABSTRACT_HAS_MANY_HPP

#include "object/object_holder.hpp"

#include <functional>

namespace oos {

namespace detail {
class object_inserter;
}

class object_store;

class abstract_has_many
{
public:
  typedef size_t size_type;

  void owner_field(const std::string &ownerfield) { owner_field_ = ownerfield; }
  void item_field(const std::string &itemfield) { item_field_ = itemfield; }

  std::string owner_field() const { return owner_field_; }
  std::string item_field() const { return item_field_; }

//  virtual void append(const object_holder &proxy) = 0;
//  virtual object_holder&& create_item() const = 0;
//  virtual size_type size() const = 0;
//  virtual void for_each(const std::function <void (const object_holder&)> &func) const = 0;

protected:
  friend class detail::object_inserter;

  object_store *ostore_ = nullptr;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";
};

}

#endif //OOS_ABSTRACT_HAS_MANY_HPP
