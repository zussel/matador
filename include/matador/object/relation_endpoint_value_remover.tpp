#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
template<class Owner>
void relation_endpoint_value_remover<Value>::remove(const object_ptr <Owner> &owner, const std::string &field, has_many_item_holder<Value> holder)
{
  field_ = field;
  holder_ = holder;

  matador::access::serialize(*this, *owner);

  field_.clear();
}

template < class Value >
void relation_endpoint_value_remover<Value>::serialize(const char *id, object_pointer<Value, object_holder_type::BELONGS_TO> &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

template < class Value >
void relation_endpoint_value_remover<Value>::serialize(const char *id, object_pointer<Value, object_holder_type::HAS_ONE> &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

template < class Value >
template < template < class ... > class Container >
void relation_endpoint_value_remover<Value>::serialize(const char *id, has_many<Value, Container> &x, cascade_type)
{
  if (field_ != id) {
    return;
  }
  x.remove_holder(holder_);
}

}
}