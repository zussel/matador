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

template < class T, class Enabled >
class table;

namespace detail {

/// @cond MATADOR_DEV

//template < class T, class Enabled = void >
//class relation_resolver;

template < class T >
class relation_resolver<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type>
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
      basic_table::t_table_map::iterator j = table_.find_table(node->type());

      if (j == table_.end_table()) {
        throw_object_exception("unknown table " << node->type());
      }
      auto k = j->second->identifier_proxy_map_.find(pk);
      if (k == j->second->identifier_proxy_map_.end()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        k = j->second->identifier_proxy_map_.insert(std::make_pair(pk, proxy)).first;
      }
      x.reset(k->second, cascade);
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
      basic_table::t_table_map::iterator j = table_.find_table(node->type());

      if (j == table_.end_table()) {
        throw_object_exception("unknown table " << node->type());
      }
      auto k = j->second->identifier_proxy_map_.find(pk);
      if (k == j->second->identifier_proxy_map_.end()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        k = j->second->identifier_proxy_map_.insert(std::make_pair(pk, proxy)).first;
      }
      x.reset(k->second, cascade);
    }
  }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &, const char *, const char *)
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
      auto endpoint = proxy_->node()->find_endpoint(id);
      if (!endpoint->second) {
        throw_object_exception("couldn't find endpoint for field " << id);
      }
      auto i = table_.has_many_relations_.find(id);
      // get relation items for id/relation
      if (i != table_.has_many_relations_.end()) {
        // get relation items for this owner identified by pk
        auto items = i->second.equal_range(id_);
        for (auto k = items.first; k != items.second; ++k) {
          endpoint->second->insert_value(has_many_item_holder<V>(*static_cast<has_many_item_holder<V>*>(k->second.get())), proxy_);
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
  object_proxy *proxy_ = nullptr;
  std::shared_ptr<basic_identifier> id_;
};

template < class T >
class relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value>::type>
{
public:
  typedef T table_type;
  
  explicit relation_resolver(basic_table &tbl)
    : table_(tbl)
  {}

  void prepare()
  {
    auto left_table_it = table_.find_table<typename table_type::left_value_type>();

    if (left_table_it == table_.end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(typename table_type::left_value_type).name()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      left_table_ = left_table_it->second;
    }


    auto right_table_it = table_.find_table<typename table_type::right_value_type>();

    if (right_table_it == table_.end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(typename table_type::right_value_type).name()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      right_table_ = right_table_it->second;
    }

  }

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
  void serialize(const char *, V &)
  {
    // must be right side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
  }

  void serialize(const char *, char *, size_t)
  {
    // must be right side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
  }

  template < class V >
  void serialize(const char *, belongs_to<V> &, cascade_type )
  {
    // check wether is left or right side value
    // left side will be determined first

    // find table of type V
    // find relation_data for type T
    //

    // if right side value it is many to many relation
  }

  template < class V >
  void serialize(const char *, has_one<V> &, cascade_type )
  {
    // must be right side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
  }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &, const char *, const char *) { }

private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  std::shared_ptr<basic_identifier> id_;

  std::shared_ptr<basic_table> left_table_;
  std::shared_ptr<basic_table> right_table_;
};

/// @endcond
}

}
#endif //OOS_RELATION_RESOLVER_HPP
