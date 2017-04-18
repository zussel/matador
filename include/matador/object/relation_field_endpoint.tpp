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
void relation_field_endpoint::create(object_store &, const object_ptr <T> &, object_store *)
{

}

template<class L, class R>
object_ptr<basic_has_many_to_many_item>
relation_field_endpoint::insert(object_store &store, const object_ptr <L> &left, const object_ptr <R> &right)
{
  return store.insert(new has_many_to_many_item<L, R>(left, right, "", ""));
}

}
}