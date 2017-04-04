#include "matador/object/relation_field_serializer.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/abstract_has_many.hpp"

namespace matador {
namespace detail {

void relation_field_serializer::serialize(const char *id, object_holder &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  if (action_ == SET) {
      x.reset(value_, cascade);
  } else if (action_ == CLEAR) {
      x.reset(nullptr, cascade);
  }
}

void relation_field_serializer::serialize(const char *id, abstract_has_many &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  if (action_ == APPEND) {
    x.append(value_);
  } else if (action_ == REMOVE) {
    x.remove(value_);
  }
}

}
}
