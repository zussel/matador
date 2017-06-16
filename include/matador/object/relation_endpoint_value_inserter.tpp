#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/object_holder.hpp"
#include "has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
template<class Owner>
void relation_endpoint_value_inserter<Value>::insert(const object_ptr <Owner> &owner, const std::string &field, object_proxy *value)
{
  std::cout << "inserting for owner " << owner.id() << " (type: " << owner.type() << ") value " << value->id() << "(" << value->node()->type() << ")\n";

  field_ = field;
  value_ = value;

  matador::access::serialize(*this, *owner);

  field_.clear();
  value_ = nullptr;
}

template < class Value >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, object_holder &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  std::cout << "setting value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";
  x.reset(value_, cascade);
}

template < class Value >
template < template < class ... > class C >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, has_many<Value, C> &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  std::cout << "inserting value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";

  typename has_many<Value, C>::holder_type holder(value_, nullptr);

  x.holder_container_.emplace_back(holder);
}

}
}