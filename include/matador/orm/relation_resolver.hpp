/**
 * @author sascha on 4/27/16.
 */

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "matador/utils/access.hpp"

#include "matador/object/has_one.hpp"
#include "matador/object/object_exception.hpp"

#include "matador/orm/basic_table.hpp"
#include "matador/orm/belongs_to_resolver.hpp"

namespace matador {

template < class T, class Enabled >
class table;

namespace detail {

/// @cond MATADOR_DEV

template < class T, class Enabled = void >
class relation_resolver;

template < class T >
class relation_resolver<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type>
{
public:
  explicit relation_resolver(basic_table &tbl)
  : table_(tbl)
  {}

  void prepare()
  {
    // build up a map with all tables for all belong_to relations
    belongs_to_resolver<T> resolver(table_);

    belongs_to_table_map_ = resolver.resolve();
  }

  void resolve(object_proxy *proxy, object_store *store)
  {
    store_ = store;
    id_ = proxy->pk();
    proxy_ = proxy;
    matador::access::serialize(*this, *proxy->obj<T>());
    proxy_ = nullptr;
    id_ = nullptr;
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
  void serialize(const char *, std::string &, size_t) { }

  template < class V >
  void serialize(const char *id, belongs_to<V> &x, cascade_type cascade);

  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type cascade)
  {
    basic_identifier *pk = x.primary_key();
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
//      std::cout << "deleting identifier " << *pk << " (field: " << id << ", " << pk << ")\n";
      delete pk;
    } else {
      /**
       * if proxy can't be found we create
       * a proxy and store it in tables
       * proxy map. it will be used when
       * table is read.
       */
      auto j = table_.find_table(node->type());

      if (j == table_.end_table()) {
        throw_object_exception("unknown table " << node->type());
      }
      auto k = j->second->find_proxy(pk);
      if (k == j->second->end_proxy()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        k = j->second->insert_proxy(pk, proxy);
        x.reset(k->second.proxy, cascade, false);
      } else {
        x.reset(k->second.proxy, cascade, true);
      }
      k->second.primary_keys.push_back(pk);
    }
  }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &x, const char *, const char *, cascade_type cascade)
  {
    serialize(id, x, cascade);
  }

  template<class V, template<class ...> class C>
  void serialize(const char *id, basic_has_many<V, C> &x, cascade_type)
  {
    // get node of object type
    prototype_iterator node = store_->find(id);
    if (node == store_->end()) {
      throw_object_exception("couldn't find prototype node");
    }

    /**
     * if relation table is loaded
     * check this tables relation proxy list
     * and update has many relation
     *
     * if relation table isn't loaded
     * append this proxy/id to relation
     * tables relation owner id list
     */

    auto data = table_.find_relation_data(id);
    if (data == table_.end_relation_data()) {
      return;
    }
    auto endpoint = proxy_->node()->find_endpoint(id);
    if (!endpoint->second) {
      throw_object_exception("couldn't find endpoint for field " << id);
    }

    if (data->second->type_index() == std::type_index(typeid(V))) {
      // correct type
      auto rdata = std::static_pointer_cast<detail::relation_data<typename basic_has_many<V, C>::value_type>>(data->second);

      rdata->insert_into_container(proxy_->pk(), x);
    }
  }

private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  basic_identifier *id_ = nullptr;

  typename belongs_to_resolver<T>::t_table_map belongs_to_table_map_;
};

template < class T >
class relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  !matador::is_builtin<typename T::right_value_type>::value
>::type>
{
public:
  typedef T table_type;
  typedef typename table_type::left_value_type left_value_type;
  typedef typename table_type::right_value_type right_value_type;

  explicit relation_resolver(basic_table &tbl)
    : table_(tbl)
  {}

  void prepare()
  {
    belongs_to_resolver<T> resolver(table_);

    auto table_map = resolver.resolve();

    basic_has_many_to_many_item* proto = table_.node().template prototype<T>();

    auto lc = proto->left_column();
    auto rc = proto->right_column();

    auto left_table_it = table_map.find(lc);
    if (left_table_it == table_map.end()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(left_value_type).name()) + " found");
    } else {
      left_table_ = left_table_it->second;
    }


    auto right_table_it = table_map.find(rc);
    if (right_table_it == table_map.end()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(right_value_type).name()) + " found");
    } else {
      right_table_ = right_table_it->second;
    }

    left_endpoint_ = table_.node().find_endpoint(std::type_index(typeid(left_value_type)))->second;
    auto right_endpoint_iterator = table_.node().find_endpoint(std::type_index(typeid(right_value_type)));

    if (right_endpoint_iterator != table_.node().endpoint_end()) {
      right_endpoint_ = right_endpoint_iterator->second;
    }

  }

  void resolve(object_proxy *proxy, object_store *store)
  {
    store_ = store;
    id_ = proxy->pk();
    proxy_ = proxy;
    left_proxy_ = nullptr;
    left_table_ptr_ = left_table_.lock();
    right_table_ptr_ = right_table_.lock();
    matador::access::serialize(*this, *proxy->obj<T>());
    proxy_ = nullptr;
    id_ = nullptr;
    store_ = nullptr;
  }

  template < class V >
  void serialize(V &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &x);

  void serialize(const char *, char *, size_t);

  template < class V >
  void serialize(const char *, belongs_to<V> &x, cascade_type cascade);

  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type cascade)
  {
    // must be left side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
    basic_identifier *pk = x.primary_key();
    if (!pk) {
      return;
    }

//    std::cout << "processing identifier " << *pk << " (has_one field: " << id << ", " << pk << ")\n";

    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);
  }
  
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) { }
  void serialize(const char *, abstract_has_many &, cascade_type) { }

private:
  object_proxy* acquire_proxy(object_holder &x, basic_identifier *pk, cascade_type cascade, const std::shared_ptr<basic_table> &tbl)
  {
    // get node of object type
    prototype_iterator node = store_->find(x.type());

    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      auto id = x.primary_key();
//      std::cout << "deleting identifier " << *id << " (" << id << ")\n";
      delete id;
      x.reset(proxy, cascade);
    } else {
      // proxy wasn't created (wasn't found in node tree)
      // find proxy in tables id(pk) proxy map
      auto id_proxy_pair = tbl->find_proxy(pk);
      if (id_proxy_pair == tbl->end_proxy()) {
//        std::cout << "relation_resolver::" << __FUNCTION__ << ": couldn't find proxy by pk" << *pk << " (" << pk << ")\n";
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        id_proxy_pair = tbl->insert_proxy(pk, proxy);
      } else {
//        std::cout << "relation_resolver::" << __FUNCTION__ << ": found proxy by pk" << *pk << " (" << pk << ")\n";
        proxy = id_proxy_pair->second.proxy;
      }
      id_proxy_pair->second.primary_keys.push_back(pk);
      x.reset(proxy, cascade);
      --(*proxy);
    }
    return proxy;
  }
private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  basic_identifier *id_ = nullptr;

  std::weak_ptr<basic_table> left_table_;
  std::weak_ptr<basic_table> right_table_;

  std::shared_ptr<basic_table> left_table_ptr_;
  std::shared_ptr<basic_table> right_table_ptr_;

  std::shared_ptr<basic_relation_endpoint> left_endpoint_;
  std::shared_ptr<basic_relation_endpoint> right_endpoint_;

  object_proxy *left_proxy_ = nullptr;
};

template < class T >
class relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  matador::is_builtin<typename T::right_value_type>::value>
::type>
{
public:
  typedef T table_type;
  typedef typename table_type::left_value_type left_value_type;
  typedef typename table_type::right_value_type right_value_type;

  explicit relation_resolver(basic_table &tbl)
    : table_(tbl)
  {}

  void prepare()
  {
    belongs_to_resolver<T> resolver(table_);

    auto table_map = resolver.resolve();

    basic_has_many_to_many_item* proto = table_.node().template prototype<T>();

    auto lc = proto->left_column();

    auto left_table_it = table_map.find(lc);

    if (left_table_it == table_map.end()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(left_value_type).name()) + " found");
    } else {
      left_table_ = left_table_it->second;
    }

    left_endpoint_ = table_.node().find_endpoint(std::type_index(typeid(left_value_type)))->second;
  }

  void resolve(object_proxy *proxy, object_store *store)
  {
    store_ = store;
    id_ = proxy->pk();
    proxy_ = proxy;
    left_proxy_ = nullptr;
    left_table_ptr_ = left_table_.lock();
    matador::access::serialize(*this, *proxy->obj<T>());
    proxy_ = nullptr;
    id_ = nullptr;
    store_ = nullptr;
  }

  template < class V >
  void serialize(V &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &x);

  void serialize(const char *, char *, size_t);

  template < class V >
  void serialize(const char *, belongs_to<V> &x, cascade_type cascade);

  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type cascade)
  {
    // must be left side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
    basic_identifier *pk = x.primary_key();
    if (!pk) {
      return;
    }

    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);
  }

//  template<class V, template<class ...> class C>
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) { }
  void serialize(const char *, abstract_has_many &, cascade_type) { }

private:
  object_proxy* acquire_proxy(object_holder &x, basic_identifier *pk, cascade_type cascade, const std::shared_ptr<basic_table> &tbl)
  {
    // get node of object type
    prototype_iterator node = store_->find(x.type());

    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      x.reset(proxy, cascade);
    } else {
      auto idproxy = tbl->find_proxy(pk);
      if (idproxy == tbl->end_proxy()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        idproxy = tbl->insert_proxy(pk, proxy);
      } else {
        proxy = idproxy->second.proxy;
      }
      idproxy->second.primary_keys.push_back(pk);
      x.reset(proxy, cascade);
    }
    return proxy;
  }
private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  basic_identifier *id_ = nullptr;

  std::weak_ptr<basic_table> left_table_;

  std::shared_ptr<basic_table> left_table_ptr_;

  std::shared_ptr<basic_relation_endpoint> left_endpoint_;

  object_proxy *left_proxy_ = nullptr;
};

/// @endcond
}

}
#endif //OOS_RELATION_RESOLVER_HPP
