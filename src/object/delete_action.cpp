//
// Created by sascha on 12.03.16.
//

#include "matador/object/delete_action.hpp"
#include "matador/object/action_visitor.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

delete_action::~delete_action()
{
  if (deleted_) {
    delete proxy_;
  }
}

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

void delete_action::backup(byte_buffer &buffer)
{
  backup_func_(buffer, this, *serializer_);
}

void delete_action::restore(byte_buffer &buffer, object_store *store)
{
  restore_func_(buffer, this, store, *serializer_);
}

void delete_action::mark_deleted()
{
  deleted_ = true;
}

}