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
bool object_deleter::is_deletable(object_proxy *proxy, const T *o) {
  objects_to_remove_.clear();
  objects_to_remove_.insert(std::make_pair(proxy->id(), t_object_count(proxy, (T*)proxy->obj())));

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
void object_deleter::serialize(const char *, const T &)
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

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

  if (!proxy_stack_.top()->node()->is_relation_node()) {
    if (x.proxy_ && x.relation_info_) {
      relations_to_remove_.push_back([&x]() {
        x.relation_info_->remove_value_from_foreign(x.owner_, x.proxy_);
      });
    }
    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1));
  }


  auto curr_obj = visited_objects_.find(proxy_stack_.top());
  if (curr_obj != visited_objects_.end()) {
    --curr_obj->second;
  }

  if (cascade & cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, (T*)x.proxy_->obj())));
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

  if (visited_objects_.find(x.proxy_) != visited_objects_.end()) {
    return;
  }

//  auto proxy = proxy_stack_.top();
//  auto current_proxy = visited_objects_.find(proxy);
//  auto &cptr = *current_proxy;
  if (!proxy_stack_.top()->node()->is_relation_node()) {
    if (x.proxy_ && x.relation_info_) {
      relations_to_remove_.push_back([&x]() {
        x.relation_info_->remove_value_from_foreign(x.owner_, x.proxy_);
      });
    }
    auto foreign_endpoint = x.relation_info_->foreign_endpoint.lock();
    if (foreign_endpoint && foreign_endpoint->type == basic_relation_endpoint::BELONGS_TO) {
      auto current_proxy = visited_objects_.find(proxy_stack_.top());
      if (current_proxy != visited_objects_.end()) {
        --current_proxy->second;
      }
    }
    visited_objects_.insert(std::make_pair(x.proxy_, x.proxy_->reference_count() - 1));

//    auto foreign_endpoint = x.relation_info_->foreign_endpoint.lock();
  }
//    if (foreign_endpoint && foreign_endpoint->type == basic_relation_endpoint::BELONGS_TO) {
//      if (current_proxy != visited_objects_.end()) {
//        --current_proxy->second;
//      }
//    }
//  } else {
//    if (current_proxy != visited_objects_.end()) {
//      --current_proxy->second;
//    }
//  }

  if (cascade & cascade_type::REMOVE) {
    objects_to_remove_.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_, (T*)x.proxy_->obj())));
    proxy_stack_.push(x.proxy_);
    matador::access::serialize(*this, *(T*)x.ptr());
    proxy_stack_.pop();
  }
}

template<class T, template<class ...> class C>
void object_deleter::serialize(const char *, basic_has_many<T, C> &x, cascade_type, typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
  typename basic_has_many<T, C>::iterator first = x.begin();
  typename basic_has_many<T, C>::iterator last = x.end();

  object_proxy *owner = proxy_stack_.top();
  while (first != last) {
    has_many_item_holder<T> &holder = first.holder_item();
    if (x.relation_info_) {
      relations_to_remove_.push_back([&holder,&x, owner]() {
        x.relation_info_->remove_value_from_foreign(holder, owner);
      });
    }

    auto curr_obj = visited_objects_.find(proxy_stack_.top());
    if (curr_obj != visited_objects_.end()) {
      --curr_obj->second;
    }
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