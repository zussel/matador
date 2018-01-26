#include "matador/object/abstract_has_many.hpp"
#include "matador/object/object_proxy.hpp"

namespace matador {

void abstract_has_many::increment_reference_count(const object_holder &holder) const
{
  ++(*(this->proxy(holder)));
}

void abstract_has_many::decrement_reference_count(const object_holder &holder) const
{
  --(*(this->proxy(holder)));
}

}