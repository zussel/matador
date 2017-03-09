#include "matador/object/object_deleter.hpp"

namespace matador {
namespace detail {

void object_deleter::t_object_count::remove(bool notify)
{
  remove_func(proxy, notify);
}

object_deleter::iterator object_deleter::begin()
{
  return object_count_map.begin();
}

object_deleter::iterator object_deleter::end()
{
  return object_count_map.end();
}

bool object_deleter::check_object_count_map() const
{
  // check the reference and pointer counter of collected objects
  const_iterator first = object_count_map.begin();
  const_iterator last = object_count_map.end();
  while (first != last) {
    if (first->second.ignore) {
      ++first;
    } else if (first->second.reference_counter == 0) {
      ++first;
    } else {
      return false;
    }
  }
  return true;
}

}
}
