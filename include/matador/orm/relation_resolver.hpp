/**
 * @author sascha on 4/27/16.
 */

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "matador/utils/access.hpp"

#include "matador/object/has_one.hpp"
#include "matador/object/object_exception.hpp"

#include "matador/orm/basic_table.hpp"

namespace matador {

template < class T >
class table;

namespace detail {

/// @cond MATADOR_DEV

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
    proxy_ = proxy;
    matador::access::serialize(*this, *proxy->obj<T>());
    proxy_ = nullptr;
    id_.reset();
    store_ = nullptr;
  }

  template < class V >
  void serialize(V &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) { }

  void serialize(const char *, char *, size_t) { }

  template < class V >
  void serialize(const char *, belongs_to<V> &x, cascade_type cascade)
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

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &x, const char *, const char *)
  {
    // get node of object type
    prototype_iterator node = store_->find(id);
    if (node == store_->end()) {
      throw_object_exception("couldn't find prototype node");
    }

    basic_table::t_table_map::iterator j = table_.find_table(node->type());

    /**
     * if relation table is loaded
     * check this tables relation proxy list
     * and update has many relation
     *
     * if relation table isn't loaded
     * append this proxy/id to relation
     * tables relation owner id list
     */
    if (j->second->is_loaded()) {
        // relation table is loaded
//      std::cout << "Todo: relation table [" << id << "/" << j->second->name() << "] loaded; append all elements for owner " << *id_ << "\n";
      auto i = table_.has_many_relations_.find(id);
      // get relation items for id/relation
      if (i != table_.has_many_relations_.end()) {
        // get relation items for this owner identified by pk
        auto items = i->second.equal_range(id_);
        for (auto k = items.first; k != items.second; ++k) {
//          typename basic_has_many<V, C>::internal_type val(k->second);
//          x.append(val);
        }
      }
    } else {
      table_.has_many_relations_.insert(std::make_pair(id, detail::t_identifier_multimap()));
      j->second->identifier_proxy_map_.insert(std::make_pair(id_, proxy_));
    }
  }

private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_;
  std::shared_ptr<basic_identifier> id_;
};

/// @endcond
}

}
#endif //OOS_RELATION_RESOLVER_HPP
