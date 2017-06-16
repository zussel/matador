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
void basic_relation_endpoint::insert_value_into_foreign(object_proxy *value, const object_ptr<T> &owner)
{
  insert_value_into_foreign(value, owner.proxy_);
}

template < class T >
void basic_relation_endpoint::remove_value_into_foreign(object_proxy *value, const object_ptr<T> &owner)
{
  remove_value_into_foreign(value, owner.proxy_);
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