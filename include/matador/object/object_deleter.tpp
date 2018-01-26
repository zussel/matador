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
  objects_to_remove_.clear();
  objects_to_remove_.insert(std::make_pair(proxy->id(), t_object_count(proxy, (T*)proxy->obj())));

//  std::cout << "check if proxy " << *proxy << " (type: " << typeid(T).name() << ") is deletable\n";
  visited_objects_.insert(std::make_pair(proxy, proxy->reference_count()));
  proxy_stack_.push(proxy);
  // start collecting information
  matador::access::serialize(*this, *o);

  proxy_stack_.pop();
  bool ret = check_object_count_map();
  visited_objects_.clear();
  return ret;
}

template<class T>
void object_deleter::serialize(const char *, T &)
{
  if (!proxy_stack_.top()->node()->is_relation_node()) {
    return;
  }
  auto curr_obj = visited_objects_.find(proxy_stack_.top());
  if (curr_obj != visited_objects_.end()) {
//    --curr_obj->second;
  }

}

template<class T>
void object_deleter::serialize(const char *, belongs_to<T> &x, cascade_type cascade)
{
  if (!x.ptr()) {
    return;
  }

//  std::cout << "checking field '" << id << "' (" << *x.proxy_ << ")\n";

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
//    std::cout << "field " << id << " already visited; skipping\n";
    return;
  }

//  std::cout << "endpoint for field " << x.relation_info_->field << " (type: " << x.relation_info_->type_name << ")\n";

  if (!proxy_stack_.top()->node()->is_relation_node()) {
    relations_to_remove_.push_back(t_relation_removal(proxy_stack_.top(), x.proxy_, x.relation_info_));

    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1)).first;
  }

//  std::cout << "current " << *proxy_stack_.top() << "\n";

  auto curr_obj = visited_objects_.find(proxy_stack_.top());
  if (curr_obj != visited_objects_.end()) {
//    x.relation_info_->contains_value_in_foreign(curr_obj->first);
    --curr_obj->second;
  }

  if (cascade & cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, (T*)x.proxy_->obj())));
//    ret.first->second.ignore = false;
    proxy_stack_.push(x.proxy_);
    matador::access::serialize(*this, *(T*)x.ptr());
    proxy_stack_.pop();
  }
}

template<class T>
void object_deleter::serialize(const char *, has_one<T> &x, cascade_type cascade)
{
  if (!x.ptr()) {
    return;
  }

//  std::cout << "checking field '" << id << "' (" << *x.proxy_ << ")\n";

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
//    std::cout << "field " << id << " already visited; skipping\n";
    return;
  }

  auto current_proxy = visited_objects_.find(proxy_stack_.top());
  if (!proxy_stack_.top()->node()->is_relation_node()) {
    relations_to_remove_.push_back(t_relation_removal(proxy_stack_.top(), x.proxy_, x.relation_info_));

    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1)).first;

    auto foreign_endpoint = x.relation_info_->foreign_endpoint.lock();
    if (foreign_endpoint && foreign_endpoint->type == basic_relation_endpoint::BELONGS_TO) {
      if (current_proxy != visited_objects_.end()) {
        --current_proxy->second;
      }
    }
  } else {
    if (current_proxy != visited_objects_.end()) {
      --current_proxy->second;
    }
  }

//  auto curr_obj = visited_objects_.find(proxy_stack_.top());
//  if (curr_obj != visited_objects_.end()) {
//    --curr_obj->second;
//  }

  if (cascade & cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, (T*)x.proxy_->obj())));
//    ret.first->second.ignore = false;
    proxy_stack_.push(x.proxy_);
    matador::access::serialize(*this, *(T*)x.ptr());
    proxy_stack_.pop();
  }
}

template<class T, template<class ...> class C>
void object_deleter::serialize(const char *, basic_has_many<T, C> &x, cascade_type, typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
//  std::cout << "checking field " << id << "\n";
  typename basic_has_many<T, C>::iterator first = x.begin();
  typename basic_has_many<T, C>::iterator last = x.end();
  while (first != last) {
    // Todo: get the real holder: on join table get has_many_item
//    object_proxy *item_proxy = first.holder_item().item_proxy();

    object_proxy *proxy = (*first).proxy_;

//    auto found_proxy = visited_objects_.find(proxy);
//    if (found_proxy != visited_objects_.end()) {
////      std::cout << "field " << id << " already visited; skipping\n";
//      ++first;
//      continue;
//    }

    relations_to_remove_.push_back(t_relation_removal(proxy, proxy_stack_.top(), x.relation_info_));

    if (!x.relation_info_->foreign_endpoint.expired()) {
      auto sptr = x.relation_info_->foreign_endpoint.lock();
      if (sptr->type == basic_relation_endpoint::HAS_MANY) {
        relations_to_remove_.push_back(t_relation_removal(proxy_stack_.top(), proxy, sptr));
      }
    }

//    if (item_proxy) {
//      objects_to_remove_.insert(std::make_pair(item_proxy->id(), t_object_count(proxy, (T *) proxy->obj())));
//    }

//    found_proxy = visited_objects_.insert(std::make_pair(proxy, proxy->reference_count() - 1)).first;

//    std::cout << "current " << *proxy_stack_.top() << "\n";
//
    auto curr_obj = visited_objects_.find(proxy_stack_.top());
    if (curr_obj != visited_objects_.end()) {
      --curr_obj->second;
    }
    /**********
     *
     * object is already in list and will
     * be ignored on deletion so set
     * ignore flag to false because this
     * node must be deleted
     *
     **********/
//    if (!ret.second && ret.first->second.ignore) {
//      ret.first->second.ignore = false;
//    }

//    auto val = *first;
//    std::cout << "check if proxy " << *proxy << " (type: " << typeid(T).name() << ") is deletable\n";
//    proxy_stack_.push(proxy);
//    matador::access::serialize(*this, *val);
//    proxy_stack_.pop();
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