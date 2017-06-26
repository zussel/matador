#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
template<class Owner>
void relation_endpoint_value_inserter<Value>::insert(const object_ptr <Owner> &owner, const std::string &field, object_proxy *value, object_proxy *item_proxy)
{
  field_ = field;
  value_ = value;
  item_proxy_ = item_proxy;

  matador::access::serialize(*this, *owner);

  item_proxy_ = nullptr;
  field_.clear();
  value_ = nullptr;
}

template < class Value >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, object_holder &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(value_, cascade, false);
}

template < class Value >
template < template < class ... > class Container >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, has_many<Value, Container> &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  typename has_many<Value, Container>::holder_type holder(object_ptr<Value>(value_), item_proxy_);

  x.insert_holder(holder);
}

}
}