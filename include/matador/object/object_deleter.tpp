#include "matador/object/object_deleter.hpp"
#include "matador/object/object_store.hpp"
namespace matador {
namespace detail {

template <typename T>
void object_deleter::t_object_count::remove_object(object_proxy *proxy, bool notify)
{
  proxy->ostore()->remove<T>(proxy, notify, false);
}

template<class T>
bool object_deleter::is_deletable(object_proxy *proxy, T *o) {
  object_count_map.clear();
  object_count_map.insert(std::make_pair(proxy->id(), t_object_count(proxy, false, (T*)proxy->obj())));

  visited_objects_.insert(proxy);
  // start collecting information
  matador::access::serialize(*this, *o);

  visited_objects_.clear();
  return check_object_count_map();
}

template<class T>
void object_deleter::serialize(const char *, belongs_to<T> &x, cascade_type cascade) {
  if (!x.ptr()) {
    return;
  }
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(
    std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, true, (T*)x.proxy_->obj()))
  );
  --ret.first->second.reference_counter;

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

  visited_objects_.insert(x.proxy_);

  if (cascade & cascade_type::REMOVE) {
    ret.first->second.ignore = false;
    matador::access::serialize(*this, *(T*)x.ptr());
  }
}

template<class T>
void object_deleter::serialize(const char *, has_one<T> &x, cascade_type cascade) {
  if (!x.ptr()) {
    return;
  }
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(
    std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, true, (T*)x.proxy_->obj()))
  );
  --ret.first->second.reference_counter;

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

  visited_objects_.insert(x.proxy_);

  if (cascade & cascade_type::REMOVE) {
    ret.first->second.ignore = false;
    matador::access::serialize(*this, *(T*)x.ptr());
  }
}

template<class T, template<class ...> class C>
void object_deleter::serialize(const char *, basic_has_many<T, C> &x, const char *, const char *, typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
  typename basic_has_many<T, C>::iterator first = x.begin();
  typename basic_has_many<T, C>::iterator last = x.end();
  while (first != last) {
    // Todo: get the real holder: on join table get has_many_item
    object_proxy *proxy = first.holder_item().item_proxy();

    std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(
      std::make_pair(proxy->id(), t_object_count(proxy, false, (T*)proxy->obj()))
    );
    /**********
     *
     * object is already in list and will
     * be ignored on deletion so set
     * ignore flag to false because this
     * node must be deleted
     *
     **********/
    if (!ret.second && ret.first->second.ignore) {
      ret.first->second.ignore = false;
    }

    if (visited_objects_.find(proxy) != visited_objects_.end()) {
      return;
    }

    visited_objects_.insert(proxy);

    auto val = *first;
    matador::access::serialize(*this, *val);
    ++first;
  }
}

template<class T>
void object_deleter::serialize(const char *id, identifier<T> &x)
{
  T val = x.value();
  serialize(id, val);
}

}
}