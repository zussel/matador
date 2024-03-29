#ifndef MATADOR_TO_MANY_ENDPOINTS_HPP
#define MATADOR_TO_MANY_ENDPOINTS_HPP

#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/has_one_to_many_item.hpp"
#include "matador/object/has_many_to_many_item.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

/*
 * Left side of a many-to-many relation
 *
 * Owner is stored in left column
 * Value is stored in right column
 */
template < class Value, class Owner >
struct left_to_many_endpoint : public from_many_endpoint<Value, Owner>
{
  typedef container_item_holder<Value> value_type;
  typedef container_item_holder<Owner> owner_type;

  left_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> owner_ptr(owner);

    std::unique_ptr<object_proxy> proxy(new object_proxy(new has_many_to_many_item<Owner, Value>(owner_ptr,
                                                                                                 holder.value(),
                                                                                                 this->owner_column,
                                                                                                 this->item_column)));
    auto item_ptr = store.insert(proxy.release(), true);
    this->increment_reference_count(holder.value());
    ++(*item_ptr);
    ++(*item_ptr);
    this->set_has_many_item_proxy(holder, item_ptr);
  }

  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
    this->decrement_reference_count(holder.value());
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(value_type(value, nullptr), owner);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(value_type(value, nullptr), owner);
  }

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    value_type value_holder(owner, owner_holder.item_proxy());
    inserter.insert(owner_holder.value(), this->field, value_holder);
  }

  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    value_type value_holder(owner, owner_holder.item_proxy());
    remover.remove(owner_holder.value(), this->field, value_holder);
  }

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }
};

/*
 * one-to-many relation endpoint
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
  typedef container_item_holder<Value> value_type;

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item_scalar<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
    ++(*owner);
  }

  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override // owner
  {
    --(*owner);
    object_ptr<has_one_to_many_item_scalar<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(container_item_holder<Owner>(owner, nullptr), value);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(container_item_holder<Owner>(owner, nullptr), value);
  }

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    inserter.insert(ownptr, this->field, static_cast<const value_type&>(holder));
  }

  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    remover.remove(ownptr, this->field, static_cast<const value_type&>(holder));
  }

  object_proxy* acquire_proxy(unsigned long long oid, object_store &store) override
  {
    if (oid == 0) {
      return nullptr;
    }
    object_proxy *proxy = store.find_proxy(oid);
    if (proxy == nullptr) {
      auto node = store.find<has_one_to_many_item_scalar<Owner, Value>>();
      auto object_type = node->object_type_entry();
      proxy = new object_proxy(node->template create<has_one_to_many_item_scalar<Owner, Value>>(), oid, object_type);
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

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> ownptr(owner);
    if (!holder.value().is_inserted()) {
      store.insert(holder.value());
    }
    std::unique_ptr<object_proxy> proxy(new object_proxy(new has_one_to_many_item_foreign<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column)));
    auto itemptr = store.insert(proxy.release(), true);
    this->increment_reference_count(holder.value());
    ++(*itemptr);
    ++(*owner);
    this->set_has_many_item_proxy(holder, itemptr);
  }

  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override // owner
  {
    --(*owner);
    object_ptr<has_one_to_many_item_foreign<Owner, Value>> itemptr(holder.item_proxy());
    store.remove(itemptr);
    this->decrement_reference_count(holder.value());
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(container_item_holder<Owner>(owner, nullptr), value);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(container_item_holder<Owner>(owner, nullptr), value);
  }

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);
    object_ptr<Owner> ownptr(owner);
    inserter.insert(ownptr, this->field, value_holder);
    this->increment_reference_count(value_holder.value());
  }

   void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);
    object_ptr<Owner> ownptr(owner);
    remover.remove(ownptr, this->field, value_holder);
    this->decrement_reference_count(value_holder.value());
  }

  object_proxy* acquire_proxy(unsigned long long oid, object_store &store) override
  {
    if (oid == 0) {
      return nullptr;
    }
    object_proxy *proxy = store.find_proxy(oid);
    if (proxy == nullptr) {
      auto node = store.find<has_one_to_many_item_foreign<Owner, Value>>();
      auto object_type = node->object_type_entry();
      proxy = new object_proxy(node->template create<has_one_to_many_item_foreign<Owner, Value>>(), oid, object_type);
    }
    return proxy;
  }

  void print(std::ostream &out) const override
  {
    out << "has_one_to_many_endpoint<" << typeid(Owner).name() << "," << typeid(Value).name() << "> relation " << this->node->type() << "::" << this->field << " (" << this->type_name << ")";
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
  typedef container_item_holder<Value> value_type;
  typedef container_item_holder<Owner> owner_type;

  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override
  {
    object_ptr<Owner> owner_ptr(owner);

    std::unique_ptr<object_proxy> proxy(new object_proxy(new has_many_to_many_item<Value, Owner>(holder.value(),
                                                                                                 owner_ptr,
                                                                                                 this->owner_column,
                                                                                                 this->item_column)));
    auto item_ptr = store.insert(proxy.release(), true);
    this->increment_reference_count(holder.value());
    ++(*item_ptr);
    ++(*item_ptr);
    this->set_has_many_item_proxy(holder, item_ptr);
  }

  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *) override // owner
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
    this->decrement_reference_count(holder.value());
  }

  void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(container_item_holder<Value>(value, nullptr), owner);
  }

  void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(container_item_holder<Value>(value, nullptr), owner);
  }

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const owner_type&>(holder);
    value_type value_holder(owner, owner_holder.item_proxy());
    inserter.insert(owner_holder.value(), this->field, value_holder);
  }

  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override
  {
    const auto &owner_holder = static_cast<const container_item_holder<Owner>&>(holder);
    value_type value_holder(owner, owner_holder.item_proxy());
    remover.remove(owner_holder.value(), this->field, value_holder);
  }

  object_proxy* acquire_proxy(unsigned long long, object_store &) override { return nullptr; }
};

/// @endcond

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
