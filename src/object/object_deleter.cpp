#include "matador/object/object_deleter.hpp"
#include "matador/object/object_exception.hpp"
#include "matador/object/object_store.hpp"

namespace matador {
namespace detail {

object_deleter::t_object_count::t_object_count(object_proxy *oproxy)
: proxy(oproxy)
{}

void object_deleter::on_primary_key(const char *id, std::string &x, size_t size)
{
  on_attribute(id, x, size);
}

void object_deleter::t_object_count::remove()
{
  proxy->ostore()->remove(proxy, false);
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

bool object_deleter::is_deletable(object_proxy *proxy) {
  objects_to_remove_.clear();
  objects_to_remove_.insert(std::make_pair(proxy->id(), t_object_count(proxy)));

  visited_objects_.insert(std::make_pair(proxy, proxy->reference_count()));
  proxy_stack_.push(proxy);
  // start collecting information
  proxy->delete_object();

  proxy_stack_.pop();
  bool ret = check_object_count_map();
  visited_objects_.clear();
  return ret;
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
