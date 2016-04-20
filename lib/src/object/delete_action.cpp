//
// Created by sascha on 12.03.16.
//

#include "object/delete_action.hpp"
#include "object/action_visitor.hpp"
#include "object/object_store.hpp"

namespace oos {

void delete_action::accept(action_visitor *av)
{
  av->visit(this);
}

const char* delete_action::classname() const
{
  return classname_.c_str();
}

basic_identifier * delete_action::pk() const
{
  return pk_.get();
}

unsigned long delete_action::id() const
{
  return id_;
}

object_proxy *delete_action::proxy() const
{
  return proxy_;
}

object_proxy* delete_action::find_proxy(object_store *store, unsigned long id)
{
  return store->find_proxy(id);
}

void delete_action::insert_proxy(object_store *store, object_proxy *proxy)
{
  store->insert_proxy(proxy);
}

}