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
    explicit relation_resolver(basic_table &tbl)
    : table_(tbl)
    {}

    void resolve(object_proxy *proxy, object_store *store)
    {
      store_ = store;
      id_ = proxy->pk();
      oos::access::serialize(*this, *proxy->obj<T>());
      id_.reset();
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

      object_proxy *proxy = node->find_proxy(pk);
      if (proxy) {
        /**
         * find proxy in node map
         * if proxy can be found object was
         * already read - replace proxy
         */
        x.reset(proxy, cascade);
      } else {
        /**
         * if proxy can't be found we create
         * a proxy and store it in tables
         * proxy map. it will be used when
         * table is read.
         */
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        basic_table::t_table_map::iterator j = table_.find_table(node->type());

        if (j == table_.end_table()) {
          throw_object_exception("unknown table " << node->type());
        }
        j->second->identifier_proxy_map_.insert(std::make_pair(pk, proxy));
        x.reset(proxy, cascade);
      }
    }

    template < class HAS_MANY >
    void serialize(const char *id, HAS_MANY &, const char *, const char *)
    {
      // get node of object type
      prototype_iterator node = store_->find(id);
      if (node == store_->end()) {
        throw_object_exception("couldn't find prototype node");
      }

      basic_table::t_table_map::iterator j = table_.find_table(node->type());

      if (j->second->is_loaded()) {
        std::cout << "relation table '" << id << "' already loaded\n";
        return;
      }

      std::cout << "found relation '" << id << "'; append object '" << *id_ << "'\n";


//      j->second->identifier_proxy_map_.insert()


    }

  private:
    object_store *store_ = nullptr;
    basic_table &table_;
    std::shared_ptr<basic_identifier> id_;
  };

}

}
#endif //OOS_RELATION_RESOLVER_HPP
