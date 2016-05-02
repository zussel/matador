/**
 * @author sascha on 4/27/16.
 */

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "tools/access.hpp"

#include "object/has_one.hpp"
#include "object/object_exception.hpp"

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
     *
     * if proxy can't be found we create
     * a proxy and store it in tables
     * proxy map. it will be used when
     * table is read.
     */
    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      x.reset(proxy, cascade);
    } else {
      proxy = new object_proxy(pk, (T*)nullptr, *node);
      basic_table::t_table_map::iterator j = table_.find_table(node->type());

      if (j == table_.end_table()) {
        throw_object_exception("unknown table " << node->type());
      }
      j->second->identifier_proxy_map_.insert(std::make_pair(pk, proxy));
    }
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
