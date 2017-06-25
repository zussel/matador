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

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
//    object_ptr<Owner> valptr(value);
    object_ptr<Owner> ownptr(value);
    inserter.insert(ownptr, this->field, owner);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Value> valptr(value);
    object_ptr<Owner> ownptr(owner);
    remover.remove(ownptr, this->field, valptr);
  }
};

template < class Value, class Owner >
using has_one_to_many_endpoint = basic_left_to_many_endpoint<Value, Owner, has_one_to_many_item>;

template < class Value, class Owner >
using left_to_many_endpoint = basic_left_to_many_endpoint<Value, Owner, has_many_to_many_item>;

template < class Value, class Owner >
struct right_to_many_endpoint : public to_many_endpoint<Value, Owner, has_many_to_many_item>
{
  right_to_many_endpoint(const std::string &field, prototype_node *node)
    : to_many_endpoint<Value, Owner, has_many_to_many_item>(field, node)
  {}

  relation_endpoint_value_inserter<Owner> inserter;
  relation_endpoint_value_remover<Owner> remover;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
//    object_ptr<Owner> valptr(value);
    object_ptr<Value> ownptr(value);
    inserter.insert(ownptr, this->field, owner);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Owner> valptr(owner);
    object_ptr<Value> ownptr(value);
    remover.remove(ownptr, this->field, valptr);
  }
};

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
