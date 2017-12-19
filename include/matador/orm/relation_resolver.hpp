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

template < class T, class Enabled = void >
class relation_resolver;

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
  void serialize(const char *id, basic_has_many<V, C> &x, const char *, const char *)
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

    auto data = table_.relation_data_map_.find(id);
    if (data == table_.relation_data_map_.end()) {
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
  std::shared_ptr<basic_identifier> id_;
};

template < class T >
class relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && !matador::is_builtin<typename T::right_value_type>::value>::type>
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
//    std::cout << "preparing table " << table_.name() << "\n";
    auto left_table_it = table_.find_table<left_value_type>();

    if (left_table_it == table_.end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(left_value_type).name()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      left_table_ = left_table_it->second;
    }


    auto right_table_it = table_.find_table<right_value_type>();

    if (right_table_it == table_.end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(right_value_type).name()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      right_table_ = right_table_it->second;
    }

//    auto left_table_ptr = left_table_.lock();
//    for(auto endpoint : left_table_ptr->node().endpoints())
//    {
//      std::cout << "left node " << left_table_ptr->node().type() << " has endpoint " << endpoint.second->field << ", type "
//                << endpoint.second->type_name;
//      auto sptr = endpoint.second->foreign_endpoint.lock();
//      if (sptr)
//        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
//      else
//        std::cout << " (no foreign endpoint)\n";
//    }

//    auto right_table_ptr = right_table_.lock();
//    for(auto endpoint : right_table_ptr->node().endpoints())
//    {
//      std::cout << "right node " << right_table_ptr->node().type() << " has endpoint " << endpoint.second->field << ", type "
//                << endpoint.second->type_name;
//      auto sptr = endpoint.second->foreign_endpoint.lock();
//      if (sptr)
//        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
//      else
//        std::cout << " (no foreign endpoint)\n";
//    }

    left_endpoint_ = table_.node().find_endpoint(std::type_index(typeid(left_value_type)))->second;
//    std::cout << *left_endpoint_ << "\n";
    auto right_endpoint_iterator = table_.node().find_endpoint(std::type_index(typeid(right_value_type)));

    if (right_endpoint_iterator != table_.node().endpoint_end()) {
      right_endpoint_ = right_endpoint_iterator->second;
//      std::cout << *right_endpoint_ << "\n";
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
    id_.reset();
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
    std::shared_ptr<basic_identifier> pk = x.primary_key();
    if (!pk) {
      return;
    }

    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);
  }
  
  template<class V, template<class ...> class C>
  void serialize(const char *, basic_has_many<V, C> &, const char *, const char *) { }

private:
  object_proxy* acquire_proxy(object_holder &x, std::shared_ptr<basic_identifier> pk, cascade_type cascade, std::shared_ptr<basic_table> tbl)
  {
    // get node of object type
    prototype_iterator node = store_->find(x.type());

    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      x.reset(proxy, cascade);
    } else {
      auto idproxy = tbl->identifier_proxy_map_.find(pk);
      if (idproxy == tbl->identifier_proxy_map_.end()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        idproxy = tbl->identifier_proxy_map_.insert(std::make_pair(pk, proxy)).first;
      } else {
        proxy = idproxy->second;
      }
      x.reset(proxy, cascade);
    }
    return proxy;
  }
private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  std::shared_ptr<basic_identifier> id_;

  std::weak_ptr<basic_table> left_table_;
  std::weak_ptr<basic_table> right_table_;

  std::shared_ptr<basic_table> left_table_ptr_;
  std::shared_ptr<basic_table> right_table_ptr_;

  std::shared_ptr<basic_relation_endpoint> left_endpoint_;
  std::shared_ptr<basic_relation_endpoint> right_endpoint_;

  object_proxy *left_proxy_ = nullptr;
};

template < class T >
class relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && matador::is_builtin<typename T::right_value_type>::value>::type>
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
//    std::cout << "preparing table " << table_.name() << "\n";
    auto left_table_it = table_.find_table<left_value_type>();

    if (left_table_it == table_.end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(typeid(left_value_type).name()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      left_table_ = left_table_it->second;
    }


//    auto left_table_ptr = left_table_.lock();
//    for(auto endpoint : left_table_ptr->node().endpoints())
//    {
//      std::cout << "left node " << left_table_ptr->node().type() << " has endpoint " << endpoint.second->field << ", type "
//                << endpoint.second->type_name;
//      auto sptr = endpoint.second->foreign_endpoint.lock();
//      if (sptr)
//        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
//      else
//        std::cout << " (no foreign endpoint)\n";
//    }

    left_endpoint_ = table_.node().find_endpoint(std::type_index(typeid(left_value_type)))->second;
//    std::cout << *left_endpoint_ << "\n";
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
    id_.reset();
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
    std::shared_ptr<basic_identifier> pk = x.primary_key();
    if (!pk) {
      return;
    }

    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);
  }

  template<class V, template<class ...> class C>
  void serialize(const char *, basic_has_many<V, C> &, const char *, const char *) { }

private:
  object_proxy* acquire_proxy(object_holder &x, std::shared_ptr<basic_identifier> pk, cascade_type cascade, std::shared_ptr<basic_table> tbl)
  {
    // get node of object type
    prototype_iterator node = store_->find(x.type());

    object_proxy *proxy = node->find_proxy(pk);
    if (proxy) {
      x.reset(proxy, cascade);
    } else {
      auto idproxy = tbl->identifier_proxy_map_.find(pk);
      if (idproxy == tbl->identifier_proxy_map_.end()) {
        proxy = new object_proxy(pk, (T*)nullptr, node.get());
        idproxy = tbl->identifier_proxy_map_.insert(std::make_pair(pk, proxy)).first;
      } else {
        proxy = idproxy->second;
      }
      x.reset(proxy, cascade);
    }
    return proxy;
  }
private:
  object_store *store_ = nullptr;
  basic_table &table_;
  object_proxy *proxy_ = nullptr;
  std::shared_ptr<basic_identifier> id_;

  std::weak_ptr<basic_table> left_table_;

  std::shared_ptr<basic_table> left_table_ptr_;

  std::shared_ptr<basic_relation_endpoint> left_endpoint_;

  object_proxy *left_proxy_ = nullptr;
};

/// @endcond
}

}
#endif //OOS_RELATION_RESOLVER_HPP
