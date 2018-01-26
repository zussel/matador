#include "matador/object/object_inserter.hpp"
#include "matador/object/object_store.hpp"

namespace matador {
namespace detail {

template<class T>
void object_inserter::insert(object_proxy *proxy, T *o, bool notify)
{

  notify_ = notify;

  proxy_stack_.push(proxy);

  modified_marker_ = [](object_store &store, object_proxy *oproxy) {
    store.mark_modified<T>(oproxy);
  };

  if (proxy->obj()) {
    matador::access::serialize(*this, *o);
  }
  proxy_stack_.pop();
}

template<class T>
void object_inserter::serialize(T &x)
{
  matador::access::serialize(*this, x);
}

template<class T>
void object_inserter::serialize(const char *, belongs_to<T> &x, cascade_type cascade) {
  if (x.is_inserted() || (x.proxy_ && x.proxy_->obj() == nullptr)) {
    return;
  }
  x.is_inserted_ = true;
  x.cascade_ = cascade;
  x.owner_ = proxy_stack_.top();

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(std::type_index(typeid(T)));
  if (!proxy_stack_.top()->node()->is_relation_node() && i != node->endpoint_end()) {
    x.relation_info_ = i->second;
    if (x.proxy_ && !x.relation_info_->foreign_endpoint.expired()) {
      x.relation_info_->insert_value_into_foreign(x.owner_, x.proxy_);
//      ++(*x.owner_);
    }
  }

  if (!x.proxy_) {
    return;
  }

  if (x.owner_->node()->is_relation_node()) {
    ++(*x.owner_);
  }

  ++(*x.proxy_);

  // object was seen by inserter stop inserting
  if (!object_proxies_.insert(x.proxy_).second) {
    return;
  }

  if (cascade & cascade_type::INSERT) {
    if (x.id() > 0) {
      // do the pointer count
      proxy_stack_.push(x.proxy_);
      matador::access::serialize(*this, *(T *) x.ptr());
      proxy_stack_.pop();
    } else {
      // new object
      ostore_.insert<T>(x.proxy_, notify_);
    }
  }
}

template<class T>
void object_inserter::serialize(const char *, has_one<T> &x, cascade_type cascade) {
  if (x.is_inserted() || (x.proxy_ && x.proxy_->obj() == nullptr)) {
    return;
  }
  x.is_inserted_ = true;
  x.cascade_ = cascade;
  x.owner_ = proxy_stack_.top();

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(std::type_index(typeid(T)));
  if (!proxy_stack_.top()->node()->is_relation_node() && i != node->endpoint_end()) {
    x.relation_info_ = i->second;

    if (x.proxy_ && !x.relation_info_->foreign_endpoint.expired()) {
      x.relation_info_->insert_value_into_foreign(x.owner_, x.proxy_);
//      ++(*x.owner_);
    }
  }

  if (!x.proxy_) {
    return;
  }

  if (x.owner_->node()->is_relation_node()) {
    ++(*x.owner_);
  }

  ++(*x.proxy_);

  // object was seen by inserter stop inserting
  if (!object_proxies_.insert(x.proxy_).second) {
    return;
  }

  if (cascade & cascade_type::INSERT) {
    if (x.id() > 0) {
      // do the pointer count
      proxy_stack_.push(x.proxy_);
      matador::access::serialize(*this, *(T *) x.ptr());
      proxy_stack_.pop();
    } else {
      // new object
      ostore_.insert<T>(x.proxy_, notify_);
    }
  }
}

template<class T, template<class ...> class C>
void object_inserter::serialize(const char *, basic_has_many<T, C> &x, cascade_type cascade,
                                typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
  // initialize the has many relation
  // set identifier
  // relation table name
  // owner column name
  // item column name
  if (proxy_stack_.empty()) {
    throw object_exception("no owner for has many relation");
  }

  if (x.ostore_) {
    return;
  }
  object_proxy *proxy = proxy_stack_.top();
  x.owner_id_ = proxy->pk();
  x.owner_ = proxy;
  x.ostore_ = &ostore_;
  x.mark_modified_owner_ = modified_marker_;

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(std::type_index(typeid(T)));
  if (i != node->endpoint_end()) {
    x.relation_info_ = std::static_pointer_cast<relation_endpoint<T>>(i->second);
  }

  auto first = x.begin();
  auto last = x.end();

  while (first != last) {
    auto j = first++;

    if (cascade & cascade_type::INSERT) {
      // item is not in store, insert it
      ostore_.insert(*j, false);
    }
    if (!j.holder_item().is_inserted()) {
      x.relation_info_->insert_holder(ostore_, j.holder_item(), proxy);
    }
  }
}

template<class T, template<class ...> class C>
void object_inserter::serialize(const char *, basic_has_many<T, C> &x, cascade_type,
                                typename std::enable_if<matador::is_builtin<T>::value>::type*)
{
  // initialize the has many relation
  // set identifier
  // relation table name
  // owner column name
  // item column name
  if (proxy_stack_.empty()) {
    throw object_exception("no owner for has many relation");
  }

  if (x.ostore_) {
    return;
  }
  object_proxy *proxy = proxy_stack_.top();
  x.owner_id_ = proxy->pk();
  x.owner_ = proxy;
  x.ostore_ = &ostore_;
  x.mark_modified_owner_ = modified_marker_;

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(std::type_index(typeid(T)));
  if (i != node->endpoint_end()) {
    x.relation_info_ = std::static_pointer_cast<relation_endpoint<T>>(i->second);
  }
}

}
}