#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/object_store.hpp"

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
  auto itemptr = store.insert(new HasManyItem<Value, Owner>(holder.value(), foreign, owner_column, item_column));
  this->set_has_many_item_proxy(holder, itemptr);
}

template < class Value, class Owner, template < class... > class HasManyItem >
void to_many_endpoint<Value, Owner, HasManyItem>::remove_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/)
{
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<Value, Owner, Type>::insert_holder(object_store &/*store*/, has_many_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<Value, Owner, Type>::remove_holder(object_store &/*store*/, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
{
  this->set_has_many_item_proxy(holder, nullptr);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<Value, Owner, Type>::insert_value(object_proxy *value, object_proxy *owner)
{
//  object_ptr<Value> valptr();
  object_ptr<Owner> ownptr(owner);
  std::cout << "insert value " << owner->id() << " (type: " << owner->node()->type() << ", typeid: " << typeid(Value).name() << ")\n";
  std::cout << "of owner " << value->id() << " (type: " << value->node()->type() << ", typeid: " << typeid(Owner).name() << ")\n";
  this->inserter.insert(ownptr, this->field, value);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<Value, Owner, Type>::remove_value(object_proxy *, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  object_ptr<Value> valptr;
  this->remover.remove(ownptr, this->field, valptr);
}

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::insert_holder(object_store &/*store*/, has_many_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::remove_holder(object_store &/*store*/, has_many_item_holder<Value> &holder, object_proxy */*owner*/)
{
  this->set_has_many_item_proxy(holder, nullptr);
}

}
}