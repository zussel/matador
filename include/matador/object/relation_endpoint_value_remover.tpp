#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
void relation_endpoint_value_remover<Value>::serialize(const char *id, object_holder &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  std::cout << "clearing value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";
  x.reset(nullptr, cascade, false);
}

template < class Value >
template < template < class ... > class Container >
void relation_endpoint_value_remover<Value>::serialize(const char *id, has_many<Value, Container> &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  std::cout << "removing value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";

  typename has_many<Value, Container>::holder_type holder(object_ptr<Value>(value_), nullptr);

  x.remove_holder(holder);
}

}
}