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

template < class Value, class Owner, template < class... > class HasManyItem >
struct basic_left_to_many_endpoint : public to_many_endpoint<Value, Owner, HasManyItem>
{
  basic_left_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner, HasManyItem>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
//    object_ptr<Owner> valptr(value);
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

//template < class Value, class Owner >
//using has_one_to_many_endpoint = basic_left_to_many_endpoint<Value, Owner, has_one_to_many_item>;

template < class Value, class Owner >
struct has_one_to_many_endpoint : public to_many_endpoint<Value, Owner, has_one_to_many_item>
{
  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner, has_one_to_many_item>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    // insert new item
//    auto itemptr = store.insert(new has_many_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    auto itemptr = store.insert(new has_one_to_many_item<Value, Owner>(holder.value(), ownptr, this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/)
  {}

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
//    object_ptr<Owner> valptr(value);
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

template < class Value, class Owner >
using left_to_many_endpoint = basic_left_to_many_endpoint<Value, Owner, has_many_to_many_item>;

template < class Value, class Owner >
struct right_to_many_endpoint : public to_many_endpoint<Value, Owner, has_many_to_many_item>
{
  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner, has_many_to_many_item>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    // insert new item
//    auto itemptr = store.insert(new has_many_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    auto itemptr = store.insert(new has_many_to_many_item<Value, Owner>(holder.value(), ownptr, this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/)
  {}

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override
  {
//    object_ptr<Owner> valptr(value);
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
