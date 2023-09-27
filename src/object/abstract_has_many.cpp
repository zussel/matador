#include "matador/object/abstract_container.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/basic_container_item_holder.hpp"

namespace matador {

void abstract_container::increment_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    ++(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

void abstract_container::decrement_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    --(*(this->proxy(holder)));
  }
}

void abstract_container::mark_holder_as_inserted(basic_container_item_holder &holder)
{
  holder.is_inserted_ = true;
}

void abstract_container::mark_holder_as_removed(basic_container_item_holder &holder)
{
  holder.is_inserted_ = false;
}

}