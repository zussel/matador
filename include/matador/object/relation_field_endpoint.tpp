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

template < class T >
void basic_relation_endpoint::insert_value_into_foreign(const object_ptr<T> &value, object_proxy *owner)
{
  insert_value_into_foreign(value.proxy_, owner);
}

template < class T >
void basic_relation_endpoint::remove_value_from_foreign(const object_ptr <T> &value, object_proxy *owner)
{
  remove_value_from_foreign(value.proxy_, owner);
}

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
  inserter.insert(ownptr, this->field, value);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<Value, Owner, Type>::remove_value(object_proxy *, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  object_ptr<Value> valptr;
  remover.remove(ownptr, this->field, valptr);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner>::insert_holder(object_store &, has_many_item_holder <Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner>::remove_holder(object_store &, has_many_item_holder <Value> &holder, object_proxy */*owner*/)
{
  this->set_has_many_item_proxy(holder, nullptr);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner>::insert_value(object_proxy *value, object_proxy *owner)
{
  object_ptr<Value> ownptr(value);
//  object_ptr<Owner> ownptr(owner);
  inserter.insert(ownptr, this->field, owner);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner>::remove_value(object_proxy *value, object_proxy *owner)
{
  object_ptr<Value> ownptr(value);
  object_ptr<Owner> valptr(owner);
  remover.remove(ownptr, this->field, valptr);
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

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::insert_value(object_proxy *value, object_proxy *owner)
{
//    object_ptr<Value> valptr(value);
  object_ptr<Owner> ownptr(owner);
  inserter.insert(ownptr, this->field, value);
}

template < class Value, class Owner >
void to_one_endpoint<Value, Owner>::remove_value(object_proxy *value, object_proxy *owner)
{
  object_ptr<Value> valptr(value);
  object_ptr<Owner> ownptr(owner);
  remover.remove(ownptr, this->field, valptr);
}

}
}