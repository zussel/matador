#include "matador/object/object_inserter.hpp"

namespace matador {
namespace detail {

object_inserter::object_inserter(object_store &ostore)
  : ostore_(ostore) { }

object_inserter::~object_inserter() { }

void object_inserter::reset()
{
  object_proxies_.clear();
  while (!object_proxy_stack_.empty()) {
    object_proxy_stack_.pop();
  }
}

}
}
