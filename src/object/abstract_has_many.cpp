#include "matador/object/abstract_has_many.hpp"
#include "matador/object/object_proxy.hpp"

namespace matador {

void abstract_has_many::increment_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    ++(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

void abstract_has_many::decrement_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    --(*(this->proxy(holder)));
  }
}

void abstract_has_many::mark_holder_as_inserted(basic_has_many_item_holder &holder) const
{
  holder.is_inserted_ = true;
}

void abstract_has_many::mark_holder_as_removed(basic_has_many_item_holder &holder) const
{
  holder.is_inserted_ = false;
}

}