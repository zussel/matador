//
// Created by sascha on 12.03.16.
//

#include "matador/object/update_action.hpp"
#include "matador/object/action_visitor.hpp"

namespace matador {

void update_action::accept(action_visitor *av)
{
  av->visit(this);
}

object_proxy* update_action::proxy()
{
  return proxy_;
}

const object_proxy* update_action::proxy() const
{
  return proxy_;
}

void update_action::backup(byte_buffer &buffer)
{
  backup_func_(buffer, this, *serializer_);
}

void update_action::restore(byte_buffer &buffer, object_store *store)
{
  restore_func_(buffer, this, store, *serializer_);
}

delete_action *update_action::release_delete_action()
{
  return delete_action_.release();
}

}