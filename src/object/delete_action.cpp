#include "matador/object/delete_action.hpp"
#include "matador/object/action_visitor.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

void delete_action::restore_delete(byte_buffer &buffer, delete_action *act, object_store *store, object_serializer &serializer)
{
  // check if there is a serializable with id in
  // serializable store
  object_proxy *proxy = store->find_proxy(act->id());
  if (!proxy) {
    // create proxy
    proxy = act->proxy_;
    store->insert_proxy(proxy);
  } else {
    act->mark_deleted();
  }
  if (!proxy->obj()) {
    proxy->create_object();
    // create serializable with id and deserialize
    // data from buffer into serializable
    // restore_ pk
    if (act->pk()) {
      proxy->pk(act->pk()->clone());
    }
    // insert serializable
  }
  proxy->restore(buffer, store, serializer);
}

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
  return pk_;
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
  proxy()->backup(buffer, *serializer_);
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