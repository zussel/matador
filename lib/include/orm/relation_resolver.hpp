/**
 * @author sascha on 4/27/16.
 */

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "tools/access.hpp"

#include "object/has_one.hpp"

#include "orm/basic_table.hpp"

namespace oos {

template < class T >
class table;

namespace detail {

template < class T >
class relation_resolver
{
public:
  explicit relation_resolver(table<T> &tbl)
    : table_(tbl)
  {}

  void resolve(T *obj, object_store *store)
  {
    store_ = store;
    oos::access::serialize(*this, *obj);
    store_ = nullptr;
  }

  template < class V >
  void serialize(V &obj)
  {
    oos::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) { }

  void serialize(const char *, char *, size_t) { }

  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type cascade)
  {
    std::shared_ptr<basic_identifier> pk = x.primary_key();
    if (!pk) {
      return;
    }

    // get node of object type
    prototype_iterator node = store_->find(x.type());

    /**
     * find proxy in node map
     * if proxy can be found object was
     * already read - replace proxy
     */
    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      x.reset(proxy, cascade);
    } else {
      // TODO: Can we call reset instead?
//      x.proxy_->obj_ = new V;
//      x.proxy_->node_ = node.get();
//      proxy = store_.register_proxy(x.proxy_);
    }

    /*
     * add the child serializable to the serializable proxy
     * of the parent container
     */

    table_.node_->update_relation(node, proxy);

    /*
     * find table of node of foreign object (this has_one)
     * try to find relations entry in containing/parent objects node
     * if found append new proxy for containing/parent object to
     */
    basic_table::t_table_map::iterator j = table_.find_table(node->type());
//    prototype_node::field_prototype_map_t::const_iterator i = table_.node_->relations.find(node->type);
//    if (i != table_.node_.relations.end()) {
//      j->second->relation_data[i->second.second][proxy->id()].push_back(new_proxy_);
//    }
  }

  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *)
  {

  }

private:
  object_store *store_ = nullptr;
  table<T> &table_;
};

}

}
#endif //OOS_RELATION_RESOLVER_HPP
