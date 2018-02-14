#include "matador/object/object_inserter.hpp"

namespace matador {
namespace detail {

object_inserter::object_inserter(object_store &ostore)
  : ostore_(ostore) { }

void object_inserter::reset()
{
  object_proxies_.clear();
  while (!proxy_stack_.empty()) {
    proxy_stack_.pop();
  }
}

void object_inserter::increment_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    ++(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

void object_inserter::decrement_reference_count(object_holder &holder) const
{
  if (holder.valid()) {
    --(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

}
}
