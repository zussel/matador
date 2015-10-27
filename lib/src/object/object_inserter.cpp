//
// Created by sascha on 10/27/15.
//
#include "object/object_inserter.hpp"
#include "object/object_ptr.hpp"

namespace oos {

object_inserter::object_inserter()
  : generic_object_reader<object_inserter>(this)
{}

object_inserter::~object_inserter()
{}

object_inserter::iterator object_inserter::begin()
{
  return object_proxies_.begin();
}

object_inserter::iterator object_inserter::end()
{
  return object_proxies_.end();
}

void object_inserter::read_value(const char */*id*/, object_base_ptr &x)
{
  if (!x.ptr()) {
    return;
  }
  if (object_proxies_.insert(x.proxy_).second) {
    // successfully insertion
    x.ptr()->deserialize(*this);
  }
}

void object_inserter::read_value(const char */*id*/, object_container &/*x*/)
{
}

}
