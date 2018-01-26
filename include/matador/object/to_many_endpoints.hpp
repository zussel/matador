//
// Created by sascha on 6/13/17.
//

#ifndef MATADOR_TO_MANY_ENDPOINTS_HPP
#define MATADOR_TO_MANY_ENDPOINTS_HPP

#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/has_one_to_many_item.hpp"
#include "matador/object/has_many_to_many_item.hpp"

namespace matador {
namespace detail {

/*
 * Left side of a many to many relation
 *
 * Owner is stored in left column
 * Value is stored in right column
 */
template < class Value, class Owner >
struct left_to_many_endpoint : public from_many_endpoint<Value, Owner>
{
  typedef has_many_item_holder<Value> value_type;
  typedef has_many_item_holder<Owner> owner_type;

  left_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(value_type(value, nullptr), owner);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(value_type(value, nullptr), owner);
  }

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    this->mark_holder_as_inserted(const_cast<owner_type&>(owner_holder));
    inserter.insert(owner_holder.value(), this->field, value_type(owner, owner_holder.item_proxy()));
//    ++(*owner);
//    this->increment_reference_count(owner_holder.value());
  }

  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    remover.remove(owner_holder.value(), this->field, value_type(owner, owner_holder.item_proxy()));
//    --(*owner);
//    this->decrement_reference_count(owner_holder.value());
  }

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override
  {
    // cast to real type object pointer
    object_ptr<Owner> ownptr(owner);
    // insert new item
    std::unique_ptr<object_proxy> proxy(new object_proxy(new has_many_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column)));
    auto itemptr = store.insert<has_many_to_many_item<Owner, Value>>(proxy.release(), true);
    this->set_has_many_item_proxy(holder, itemptr);
//    this->increment_reference_count(holder.value());
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
//    this->decrement_reference_count(holder.value());
  }

  object_proxy* acquire_proxy(unsigned long , object_store &) override
  {
    return nullptr;
  }
};

/*
 * One to many relation endpoint
 *
 * Owner is stored in left column
 * Value is stored in right column
 */
template < class Owner, class Value, class Enabled = void >
struct has_one_to_many_endpoint;

template < class Owner, class Value >
struct has_one_to_many_endpoint<Owner, Value, typename std::enable_if<matador::is_builtin<Value>::value>::type>
  : public from_many_endpoint<Value, Owner>
{
  typedef has_many_item_holder<Value> value_type;

  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_one_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    inserter.insert(ownptr, this->field, static_cast<const value_type&>(holder));
  }

  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    remover.remove(ownptr, this->field, static_cast<const value_type&>(holder));
  }

  object_proxy* acquire_proxy(unsigned long oid, object_store &store) override
  {
    if (oid == 0) {
      return nullptr;
    }
    object_proxy *proxy = store.find_proxy(oid);
    if (proxy == nullptr) {
      proxy = new object_proxy(new has_one_to_many_item<Owner, Value>(), oid, &store);
    }
    return proxy;
  }
};

template < class Owner, class Value >
struct has_one_to_many_endpoint<Owner, Value, typename std::enable_if<!matador::is_builtin<Value>::value>::type>
  : public from_many_endpoint<Value, Owner>
{

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_one_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &value_holder = static_cast<const has_many_item_holder<Value>&>(holder);
    object_ptr<Owner> ownptr(owner);
    inserter.insert(ownptr, this->field, value_holder);
    this->increment_reference_count(value_holder.value());
  }

   void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &value_holder = static_cast<const has_many_item_holder<Value>&>(holder);
    object_ptr<Owner> ownptr(owner);
    remover.remove(ownptr, this->field, value_holder);
    this->decrement_reference_count(value_holder.value());
  }

  object_proxy* acquire_proxy(unsigned long oid, object_store &store) override
  {
    if (oid == 0) {
      return nullptr;
    }
    object_proxy *proxy = store.find_proxy(oid);
    if (proxy == nullptr) {
      proxy = new object_proxy(new has_one_to_many_item<Owner, Value>(), oid, &store);
    }
    return proxy;
  }
};

/*
 * Right side of a many to many relation
 *
 * Owner is stored in right column
 * Value is stored in left column
 */
template < class Value, class Owner >
struct right_to_many_endpoint : public from_many_endpoint<Value, Owner>
{
  typedef has_many_item_holder<Value> value_type;
  typedef has_many_item_holder<Owner> owner_type;

  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);

    std::unique_ptr<object_proxy> proxy(new object_proxy(new has_many_to_many_item<Value, Owner>(holder.value(), ownptr, this->owner_column, this->item_column)));
    auto itemptr = store.insert<has_many_to_many_item<Owner, Value>>(proxy.release(), true);
    this->set_has_many_item_proxy(holder, itemptr);
    // inc value ref count
//    this->increment_reference_count(holder.value());
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
//    this->decrement_reference_count(holder.value());
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Value>(value, nullptr), owner);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Value>(value, nullptr), owner);
  }

  void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    this->mark_holder_as_inserted(const_cast<owner_type&>(owner_holder));
    inserter.insert(owner_holder.value(), this->field, value_type(owner, owner_holder.item_proxy()));
//    ++(*owner);
//    this->increment_reference_count(owner_holder.value());
  }

  void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const has_many_item_holder<Owner>&>(holder);
    remover.remove(owner_holder.value(), this->field, value_type(owner, owner_holder.item_proxy()));
//    --(*owner);
//    this->decrement_reference_count(owner_holder.value());
  }

  object_proxy* acquire_proxy(unsigned long , object_store &) override
  {
    return nullptr;
  }

};

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
