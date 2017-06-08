#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/has_one_to_many_item.hpp"
#include "matador/object/has_many_to_many_item.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_many_to_many_item.hpp"

namespace matador {
namespace detail {

template<class T>
void basic_relation_endpoint::set_has_many_item_proxy(has_many_item_holder<T> &holder, const object_holder &obj)
{
  set_has_many_item_proxy(holder, obj.proxy_);
}

template<class T>
void basic_relation_endpoint::set_has_many_item_proxy(has_many_item_holder<T> &holder, object_proxy *proxy)
{
  holder.has_many_to_many_item_poxy_ = proxy;
}

template < class Value, class Owner >
using has_one_to_many_endpoint = to_many_endpoint<Value, Owner, has_one_to_many_item >;

template < class Value, class Owner >
using has_many_to_many_endpoint = to_many_endpoint<Value, Owner, has_many_to_many_item >;

//template < class T >
//void relation_field_endpoint::set(object_store &store, const object_ptr<T> &owner, object_proxy *value)
//{
//  if (is_insert_in_progress) {
//    return;
//  }
//  is_insert_in_progress = true;
//
//
//  store.mark_modified(owner);
//
//  auto sptr = foreign_endpoint.lock();
//  if (!sptr->is_insert_in_progress) {
//    store.mark_modified(owner);
//    field_serializer.set(field, owner, value);
//  }
//
//  is_insert_in_progress = false;
//}
//
//template < class T >
//void relation_field_endpoint::clear(object_store &store, const object_ptr<T> &owner)
//{
//  if (is_remove_in_progress) {
//    return;
//  }
//  is_remove_in_progress = true;
//
//  auto sptr = foreign_endpoint.lock();
//  if (!sptr->is_remove_in_progress) {
//    store.mark_modified(owner);
//    field_serializer.clear(field, owner);
//  }
//
//  is_remove_in_progress = false;
//}
//
//template < class T >
//void relation_field_endpoint::append(object_store &store, const object_ptr<T> &owner, object_proxy *value)
//{
//  if (is_insert_in_progress) {
//    return;
//  }
//  is_insert_in_progress = true;
//
//  auto sptr = foreign_endpoint.lock();
//  if (!sptr->is_insert_in_progress) {
//    store.mark_modified(owner);
//    field_serializer.append(field, owner, value);
//  }
//
//  is_insert_in_progress = false;
//}
//
//template < class T >
//void relation_field_endpoint::remove(object_store &, const object_ptr<T> &owner, object_proxy *value)
//{
//  if (is_remove_in_progress) {
//    return;
//  }
//  is_remove_in_progress = true;
//
//  auto sptr = foreign_endpoint.lock();
//  if (!sptr->is_remove_in_progress) {
////    store.mark_modified(owner);
//
//    field_serializer.remove(field, owner, value);
//  }
//
//  is_remove_in_progress = false;
//}
//
//template<class T>
//void relation_field_endpoint::create(object_store &, const object_ptr <T> &, object_proxy *owner)
//{
//
//}
//
//template< class T, class V >
//void relation_field_endpoint::insert(object_store &store, const object_ptr <T> &value, object_proxy *owner)
//{
//  if (side == LEFT) {
//    store.insert(new has_many_to_many_item<T, V>(value, object_ptr<V>(owner), "", ""));
//  } else /* side == RIGHT */ {
//    store.insert(new has_many_to_many_item<V, T>(object_ptr<V>(owner), value, "", ""));
//  }
//}
//
//template<class Value, class Owner>
//void relation_field_endpoint::insert(object_store &store, has_many_item_holder<Value> &holder)
//{
//}

template < class Value, class Owner, template < class... > class HasManyItem >
void to_many_endpoint<Value, Owner, HasManyItem>::insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
{
  // cast to real type object pointer
  object_ptr<Owner> foreign(owner);
  // insert new item
  auto itemptr = store.insert(new HasManyItem<Value, Owner>(holder.value, foreign, owner_column, item_column));
  this->set_has_many_item_proxy(holder, itemptr);
}

template < class Value, class Owner, template < class... > class HasManyItem >
void to_many_endpoint<Value, Owner, HasManyItem>::remove_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/)
{
}

template < class Value, class Owner >
void has_one_endpoint<Value, Owner>::insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner >
void has_one_endpoint<Value, Owner>::remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
{
}

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner)
{
}

}
}