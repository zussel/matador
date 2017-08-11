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
  left_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Value>(owner, nullptr), value);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Value>(owner, nullptr), value);
  }

  virtual void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    inserter.insert(
      static_cast<const has_many_item_holder<Owner>&>(holder).value(),
      this->field,
      has_many_item_holder<Value>(owner, holder.item_proxy())
    );
  }

  virtual void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    remover.remove(
      static_cast<const has_many_item_holder<Owner>&>(holder).value(),
      this->field,
      has_many_item_holder<Value>(owner, holder.item_proxy())
    );
  }

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    // cast to real type object pointer
    object_ptr<Owner> ownptr(owner);
    // insert new item
    auto itemptr = store.insert(new has_many_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_many_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  virtual object_proxy* acquire_proxy(unsigned long , object_store &) override
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
  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

//  relation_endpoint_value_inserter<Owner> inserter;
//  relation_endpoint_value_remover<Owner> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_one_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  virtual void insert_value(const basic_has_many_item_holder &/*holder*/, object_proxy */*owner*/) override
  {
//    object_ptr<Value> ownptr(owner);
//    inserter.insert(ownptr, this->field, static_cast<const has_many_item_holder<Owner>&>(holder));
  }

  virtual void remove_value(const basic_has_many_item_holder &/*holder*/, object_proxy */*owner*/) override
  {
//    object_ptr<Value> ownptr(owner);
//    remover.remove(ownptr, this->field, static_cast<const has_many_item_holder<Owner>&>(holder));
  }

  virtual object_proxy* acquire_proxy(unsigned long oid, object_store &store) override
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

  relation_endpoint_value_inserter<Owner> inserter;
  relation_endpoint_value_remover<Owner> remover;

  has_one_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
  {}

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
  {
    object_ptr<Owner> ownptr(owner);
    auto itemptr = store.insert(new has_one_to_many_item<Owner, Value>(ownptr, holder.value(), this->owner_column, this->item_column));
    this->set_has_many_item_proxy(holder, itemptr);
  }

  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
  {
    object_ptr<has_one_to_many_item<Owner, Value>> item(holder.item_proxy());
    store.remove(item);
  }

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Owner>(owner, nullptr), value);
  }

  virtual void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Value> ownptr(owner);
    inserter.insert(ownptr, this->field, static_cast<const has_many_item_holder<Owner>&>(holder));
  }

  virtual void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    object_ptr<Value> ownptr(owner);
    remover.remove(ownptr, this->field, static_cast<const has_many_item_holder<Owner>&>(holder));
  }

  virtual object_proxy* acquire_proxy(unsigned long oid, object_store &store) override
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
  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : from_many_endpoint<Value, Owner>(field, node)
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

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    insert_value(has_many_item_holder<Value>(owner, nullptr), value);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    remove_value(has_many_item_holder<Value>(owner, nullptr), value);
  }

  virtual void insert_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    inserter.insert(
      static_cast<const has_many_item_holder<Owner>&>(holder).value(),
      this->field,
      has_many_item_holder<Value>(owner, holder.item_proxy())
    );
  }

  virtual void remove_value(const basic_has_many_item_holder &holder, object_proxy *owner) override
  {
    remover.remove(
      static_cast<const has_many_item_holder<Owner>&>(holder).value(),
      this->field,
      has_many_item_holder<Value>(owner, holder.item_proxy())
    );
  }

  virtual object_proxy* acquire_proxy(unsigned long , object_store &) override
  {
    return nullptr;
  }

};

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
