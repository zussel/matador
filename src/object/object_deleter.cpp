#include "matador/object/object_deleter.hpp"

namespace matador {
namespace detail {

void object_deleter::t_object_count::remove(bool notify)
{
  remove_func(proxy, notify);
}

void object_deleter::remove(bool notify)
{
  // first clear all relations
  for (auto &relation_to_remove : relations_to_remove_) {
    relation_to_remove.remove();
  }

  // second remove all objects/proxies
  for (auto &object_to_remove : objects_to_remove_) {
    object_to_remove.second.remove(notify);
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

void object_deleter::serialize(const char *, char *, size_t)
{
  if (!proxy_stack_.top()->node()->is_relation_node()) {
    return;
  }
  auto curr_obj = visited_objects_.find(proxy_stack_.top());
  if (curr_obj != visited_objects_.end()) {
//    --curr_obj->second;
  }
}

bool object_deleter::check_object_count_map() const
{
  for (const auto &o : objects_to_remove_)
  {
    auto p = visited_objects_.find(o.second.proxy);
    if (p != visited_objects_.end()) {
      auto &pp = *p;
      if (p->second > 0) {
        return false;
      } else if (p->second < 0) {
        throw_object_exception("reference count of " << *p->first << " is less zero (" << p->second << ")");
      }
    }
  }
  return true;
}

void object_deleter::t_relation_removal::remove()
{
  if (owner && endpoint) {
    endpoint->remove_value_from_foreign(proxy, owner);
  }
}

}
}
