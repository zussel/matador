#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
template<class Owner>
void relation_endpoint_value_remover<Value,
  typename std::enable_if<
//    std::is_base_of<object_holder, Value>::value>::type
    std::is_integral<int>::value>::type
>::remove(const object_ptr <Owner> &owner, const std::string &field, has_many_item_holder<Value> holder)
{
  field_ = field;
  holder_ = holder;

  matador::access::serialize(*this, *owner);

  field_.clear();
}

template < class Value >
void relation_endpoint_value_remover<Value,
  typename std::enable_if<
//    std::is_base_of<object_holder, Value>::value>::type
    std::is_integral<int>::value>::type
>::serialize(const char *id, object_pointer<Value, object_holder_type::BELONGS_TO> &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

template < class Value >
void relation_endpoint_value_remover<Value,
  typename std::enable_if<
//    std::is_base_of<object_holder, Value>::value>::type
    std::is_integral<int>::value>::type
>::serialize(const char *id, object_pointer<Value, object_holder_type::HAS_ONE> &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

template < class Value >
template < template < class ... > class Container >
void relation_endpoint_value_remover<Value,
  typename std::enable_if<
//    std::is_base_of<object_holder, Value>::value>::type
    std::is_integral<int>::value>::type
>::serialize(const char *id, has_many<Value, Container> &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  x.remove_holder(holder_);
}

//template < class Value >
//template<class Owner>
//void relation_endpoint_value_remover<Value,
//  typename std::enable_if<
//    !std::is_base_of<object_holder, Value>::value>::type
//>::remove(const object_ptr <Owner> &owner, const std::string &field, has_many_item_holder<Value> holder)
//{
//  field_ = field;
//  holder_ = holder;
//
//  matador::access::serialize(*this, *owner);
//
//  field_.clear();
//}
//
//template < class Value >
//template < class T >
//void relation_endpoint_value_remover<Value,
//  typename std::enable_if<
//    !std::is_base_of<object_holder, Value>::value>::type
//>::serialize(const char *, T &)
//{
//}
//
//template < class Value >
//void relation_endpoint_value_remover<Value,
//  typename std::enable_if<
//    !std::is_base_of<object_holder, Value>::value>::type
//>::serialize(const char *, char *, std::size_t )
//{
//}

}
}