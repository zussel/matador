#include "matador/object/object_deleter.hpp"
#include "matador/object/object_exception.hpp"

namespace matador {
namespace detail {

void object_deleter::t_object_count::remove()
{
  remove_func(proxy);
}

void object_deleter::remove()
{
  // first clear all relations
  for (auto &relation_to_remove : relations_to_remove_) {
    relation_to_remove();
  }

  // second remove all objects/proxies
  for (auto &object_to_remove : objects_to_remove_) {
    object_to_remove.second.remove();
  }

  // cleanup
  relations_to_remove_.clear();
  objects_to_remove_.clear();
}

object_deleter::iterator object_deleter::begin()
{
  return objects_to_remove_.begin();
}

object_deleter::iterator object_deleter::end()
{
  return objects_to_remove_.end();
}

bool object_deleter::check_object_count_map() const
{
  for (const auto &o : objects_to_remove_)
  {
    auto p = visited_objects_.find(o.second.proxy);
    if (p != visited_objects_.end()) {
      if (p->second > 0) {
        return false;
      } else if (p->second < 0) {
        throw_object_exception("reference count of " << *p->first << " is less zero (" << p->second << ")");
      }
    }
  }
  return true;
}

}
}
