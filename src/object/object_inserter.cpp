#include "matador/object/object_inserter.hpp"

#include "matador/object/object_store.hpp"

namespace matador {
namespace detail {

object_inserter::object_inserter(object_store &store)
  : ostore_(store) { }

void object_inserter::insert(object_proxy *proxy, bool notify)
{
  notify_ = notify;

  proxy_stack_.push(proxy);

  modified_marker_ = [](object_store &store, object_proxy *oproxy) {
    store.mark_modified(oproxy);
  };

  if (proxy->obj()) {
    proxy->insert_object();
  }
  if (!proxy_stack_.empty()) {
    proxy_stack_.pop();
  }
}

void object_inserter::reset()
{
  object_proxies_.clear();
  while (!proxy_stack_.empty()) {
    proxy_stack_.pop();
  }
}

void object_inserter::increment_reference_count(const object_holder &holder) const
{
  if (holder.valid()) {
    ++(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

void object_inserter::decrement_reference_count(object_holder &holder) const
{
  if (holder.valid()) {
    --(*(this->proxy(const_cast<object_holder &>(holder))));
  }
}

void object_inserter::insert_object(object_holder &x, const std::type_index &type_index, cascade_type cascade)
{
  if (x.is_inserted()) {
    return;
  }

  x.is_inserted_ = true;
  x.cascade_ = cascade;
  x.owner_ = proxy_stack_.top();

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(type_index);
  if (!proxy_stack_.top()->node()->is_relation_node() && i != node->endpoint_end()) {
    x.relation_info_ = i->second;

    if (x.proxy_ && x.proxy_->obj() != nullptr) {
      if (i->second->type == basic_relation_endpoint::HAS_ONE/* x.is_has_one()*/) {
        ++(*x.proxy_);
      }
      if (!x.relation_info_->foreign_endpoint.expired()) {
        x.relation_info_->insert_value_into_foreign(x.owner_, x.proxy_);
      }
    }
  }

  if (!x.proxy_ || x.proxy_->obj() == nullptr) {
    return;
  }

  if (!proxy_stack_.top()->node()->is_relation_node() && i->second->type == basic_relation_endpoint::BELONGS_TO/*x.is_belongs_to()*/) {
    ++(*x.proxy_);
  }

  // object was seen by inserter stop inserting
  if (!object_proxies_.insert(x.proxy_).second) {
    return;
  }

  if ((cascade & cascade_type::INSERT) != cascade_type::NONE) {
    if (x.id() > 0) {
      // do the pointer count
      proxy_stack_.push(x.proxy_);
      x.proxy_->insert_object();
      proxy_stack_.pop();
    } else {
      // new object
      ostore_.insert(x.proxy_, notify_);
    }
  }
}

void object_inserter::insert_proxy(object_proxy *proxy)
{
  ostore_.insert(proxy, true);
}

object_proxy* object_inserter::initialize_has_many(abstract_container &x)
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
    return nullptr;
  }
  object_proxy *proxy = proxy_stack_.top();
  x.owner_id_ = proxy->pk();
  x.owner_ = proxy;
  x.ostore_ = &ostore_;
  x.mark_modified_owner_ = modified_marker_;

  return proxy;
}

}
}
