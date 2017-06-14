#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/object_holder.hpp"

namespace matador {
namespace detail {

template < class Value >
void relation_endpoint_value_remover<Value>::serialize(const char *id, object_holder &x, cascade_type)
{
  if (field_ != id) {
    return;
  }
  std::cout << "clearing value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";
  x.clear();
}

template < class Value >
void relation_endpoint_value_remover<Value>::serialize(const char *id, abstract_has_many &, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  std::cout << "removing value (" << typeid(Value).name() << ") for field [" << field_ << "]\n";
}

}
}