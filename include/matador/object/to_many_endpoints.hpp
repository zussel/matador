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
struct left_to_many_endpoint : public to_many_endpoint<Value, Owner>
{
  left_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Owner> ownptr(value);
    inserter.insert(ownptr, this->field, owner, item_proxy);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Value> valptr(owner);
    object_ptr<Owner> ownptr(value);
    remover.remove(ownptr, this->field, valptr, item_proxy);
  }

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    // cast to real type object pointer
    object_ptr<Owner> ownptr(owner);
    // insert new item
    auto itemptr = store.insert(new has_one_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_one_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }
};

/*
 * One to many relation endpoint
 *
 * Owner is stored in left column
 * Value is stored in right column
 */
template < class Value, class Owner >
struct has_one_to_many_endpoint : public to_many_endpoint<Value, Owner>
{
  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item<Value, Owner>(holder.value(), ownptr, this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Owner> ownptr(value);
    inserter.insert(ownptr, this->field, owner, item_proxy);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Value> valptr(owner);
    object_ptr<Owner> ownptr(value);
    remover.remove(ownptr, this->field, valptr, item_proxy);
  }
};

/*
 * Right side of a many to many relation
 *
 * Owner is stored in right column
 * Value is stored in left column
 */
template < class Value, class Owner >
struct right_to_many_endpoint : public to_many_endpoint<Value, Owner>
{
  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_many_to_many_item<Value, Owner>(holder.value(), ownptr, this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Owner> ownptr(value);
    inserter.insert(ownptr, this->field, owner, item_proxy);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
    object_ptr<Value> valptr(owner);
    object_ptr<Owner> ownptr(value);
    remover.remove(ownptr, this->field, valptr, item_proxy);
  }
};

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
