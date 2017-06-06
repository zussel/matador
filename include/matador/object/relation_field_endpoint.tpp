#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_many_to_many_item.hpp"

namespace matador {
namespace detail {

template < class T >
void relation_field_endpoint::set(object_store &store, const object_ptr<T> &owner, object_proxy *value)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;


  store.mark_modified(owner);

  auto sptr = foreign_endpoint.lock();
  if (!sptr->is_insert_in_progress) {
    store.mark_modified(owner);
    field_serializer.set(field, owner, value);
  }

  is_insert_in_progress = false;
}

template < class T >
void relation_field_endpoint::clear(object_store &store, const object_ptr<T> &owner)
{
  if (is_remove_in_progress) {
    return;
  }
  is_remove_in_progress = true;

  auto sptr = foreign_endpoint.lock();
  if (!sptr->is_remove_in_progress) {
    store.mark_modified(owner);
    field_serializer.clear(field, owner);
  }

  is_remove_in_progress = false;
}

template < class T >
void relation_field_endpoint::append(object_store &store, const object_ptr<T> &owner, object_proxy *value)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;

  auto sptr = foreign_endpoint.lock();
  if (!sptr->is_insert_in_progress) {
    store.mark_modified(owner);
    field_serializer.append(field, owner, value);
  }

  is_insert_in_progress = false;
}

template < class T >
void relation_field_endpoint::remove(object_store &, const object_ptr<T> &owner, object_proxy *value)
{
  if (is_remove_in_progress) {
    return;
  }
  is_remove_in_progress = true;

  auto sptr = foreign_endpoint.lock();
  if (!sptr->is_remove_in_progress) {
//    store.mark_modified(owner);

    field_serializer.remove(field, owner, value);
  }

  is_remove_in_progress = false;
}

template<class T>
void relation_field_endpoint::create(object_store &, const object_ptr <T> &, object_proxy *owner)
{

}

template< class T, class V >
void relation_field_endpoint::insert(object_store &store, const object_ptr <T> &value, object_proxy *owner)
{
  if (side == LEFT) {
    store.insert(new has_many_to_many_item<T, V>(value, object_ptr<V>(owner), "", ""));
  } else /* side == RIGHT */ {
    store.insert(new has_many_to_many_item<V, T>(object_ptr<V>(owner), value, "", ""));
  }
}

template<class Value, class Owner>
void relation_field_endpoint::insert(object_store &store, has_many_item_holder<Value> &holder)
{

}
}
}