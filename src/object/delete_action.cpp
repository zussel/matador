#include "matador/object/delete_action.hpp"
#include "matador/object/action_visitor.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

delete_action::delete_action(const char* classname, unsigned long id, object_proxy *proxy)
  : classname_(classname)
  , id_(id)
  , proxy_(proxy)
{}

delete_action::delete_action(object_proxy *proxy)
  : delete_action(proxy->node()->type(), proxy->id(), proxy)
{}

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
  return proxy_->pk();
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
  // check if there is a serializable with id in
  // serializable store
  object_proxy *proxy = store->find_proxy(id());
  if (!proxy) {
    // create proxy
    proxy = proxy_;
    store->insert_proxy(proxy);
  } else {
    mark_deleted();
  }
  if (!proxy->obj()) {
    proxy->create_object();
    // create serializable with id and deserialize
    // data from buffer into serializable
    // restore_ pk
    if (pk()) {
      proxy->pk(pk()->clone());
    }
    // insert serializable
  }
  proxy->restore(buffer, *deserializer_);
}

void delete_action::mark_deleted()
{
  deleted_ = true;
}

}