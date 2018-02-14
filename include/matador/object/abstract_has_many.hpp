//
// Created by sascha on 3/17/16.
//

#ifndef OOS_ABSTRACT_HAS_MANY_HPP
#define OOS_ABSTRACT_HAS_MANY_HPP

#include "matador/object/object_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include <functional>

namespace matador {

/**
 * @brief Detail namespace of the open object store.
 *
 * This is the namespace where for all function, classes
 * and other code which is not neccessary to be public should go.
 */
namespace detail {
class object_inserter;
}

template < class T, object_holder_type OHT >
class object_pointer;
class object_store;
class object_proxy;

/// @cond MATADOR_DEV

class abstract_has_many : public detail::object_proxy_accessor
{
public:
  typedef size_t size_type;

  void owner_field(const std::string &ownerfield) { owner_field_ = ownerfield; }
  void item_field(const std::string &itemfield) { item_field_ = itemfield; }

  std::string owner_field() const { return owner_field_; }
  std::string item_field() const { return item_field_; }

  object_store* store() const { return ostore_; }

protected:
  template < class T >
  void increment_reference_count(T) const {}
  template < class T >
  void increment_reference_count(const object_pointer<T, object_holder_type::OBJECT_PTR> &pointer) const
  {
    increment_reference_count(static_cast<const object_holder&>(pointer));
  }
  void increment_reference_count(const object_holder &holder) const;
  template < class T >
  void decrement_reference_count(T) const {}
  template < class T >
  void decrement_reference_count(const object_pointer<T, object_holder_type::OBJECT_PTR> &pointer) const
  {
    decrement_reference_count(static_cast<const object_holder&>(pointer));
  }
  void decrement_reference_count(const object_holder &holder) const;

  void mark_holder_as_inserted(basic_has_many_item_holder &holder) const;
  void mark_holder_as_removed(basic_has_many_item_holder &holder) const;

protected:
  friend class detail::object_inserter;

  object_store *ostore_ = nullptr;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";
};

/// @endcond

}

#endif //OOS_ABSTRACT_HAS_MANY_HPP
