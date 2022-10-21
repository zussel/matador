#include "matador/object/delete_action.hpp"
#include "matador/object/action_visitor.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

template < class T, class S >
void delete_action::backup_delete(byte_buffer &buffer, delete_action *act, S &serializer)
{
  T* obj = (T*)(act->proxy()->obj());
  act->backup_object(obj);
  serializer.serialize(obj, &buffer);
}

template < class T, class S >
void delete_action::restore_delete(byte_buffer &buffer, delete_action *act, object_store *store, S &serializer)
{
  // check if there is a serializable with id in
  // serializable store
  object_proxy *proxy = action::find_proxy(store, act->id());
  if (!proxy) {
    // create proxy
    proxy = act->proxy_;
//      proxy = new object_proxy(new T, act->id(), store);
    action::insert_proxy(store, proxy);
  } else {
    act->mark_deleted();
  }
//    object_serializer serializer;
  if (!proxy->obj()) {
    // create serializable with id and deserialize
    proxy->restore_delete(buffer, store);
    T *obj = act->init_object(new T);
    proxy->reset(obj);
    // data from buffer into serializable
    serializer.deserialize(obj, &buffer, store);
    // restore pk
    if (act->pk()) {
      proxy->pk(act->pk()->clone());
    }
    // insert serializable
  } else {
    // data from buffer into serializable
    T *obj = act->init_object((T*)proxy->obj());
    serializer.deserialize(obj, &buffer, store);
  }
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