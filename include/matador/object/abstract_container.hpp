#ifndef OOS_ABSTRACT_HAS_MANY_HPP
#define OOS_ABSTRACT_HAS_MANY_HPP

#include "matador/object/export.hpp"

#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/identifier.hpp"

#include <functional>
#include <string>

namespace matador {

class object_holder;

/**
 * @brief Detail namespace of the open object store.
 *
 * This is the namespace where for all function, classes
 * and other code which is not necessary to be public should go.
 */
namespace detail {
class object_inserter;
}

class basic_container_item_holder;
template < class T >
class object_ptr;
class object_store;
class object_proxy;

/// @cond MATADOR_DEV

class MATADOR_OBJECT_API abstract_container : public detail::object_proxy_accessor
{
public:
  using mark_modified_owner_func = std::function<void(object_store&, object_proxy*)>; /**< Shortcut to mark modified owner function */

public:
  typedef size_t size_type;

  void owner_field(const std::string &owner_field) { owner_field_ = owner_field; }
  void item_field(const std::string &item_field) { item_field_ = item_field; }

  std::string owner_field() const { return owner_field_; }
  std::string item_field() const { return item_field_; }

  object_store* store() const { return ostore_; }

protected:
  template < class T >
  void increment_reference_count(T) const {}
  template < class T >
  void increment_reference_count(const object_ptr<T> &pointer) const
  {
    increment_reference_count(static_cast<const object_holder&>(pointer));
  }
  void increment_reference_count(const object_holder &holder) const;
  template < class T >
  void decrement_reference_count(T) const {}
  template < class T >
  void decrement_reference_count(const object_ptr<T> &pointer) const
  {
    decrement_reference_count(static_cast<const object_holder&>(pointer));
  }
  void decrement_reference_count(const object_holder &holder) const;

  static void mark_holder_as_inserted(basic_container_item_holder &holder) ;
  static void mark_holder_as_removed(basic_container_item_holder &holder) ;

protected:
  friend class detail::object_inserter;

  object_store *ostore_ = nullptr;

  object_proxy *owner_ = nullptr;
  identifier owner_id_;

  mark_modified_owner_func mark_modified_owner_;

  std::string owner_field_ = "owner_id";
  std::string item_field_ = "item_id";
};

/// @endcond

}

#endif //OOS_ABSTRACT_HAS_MANY_HPP
