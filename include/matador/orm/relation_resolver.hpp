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

    auto p = data->second.get();
//    data->second->insert(proxy_, endpoint);
//      table_.has_many_relations_.insert(std::make_pair(id, detail::t_identifier_multimap()));
//      j->second->identifier_proxy_map_.insert(std::make_pair(id_, proxy_));
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
    std::cout << "preparing table " << table_.name() << "\n";
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

    auto left_table_ptr = left_table_.lock();
    for(auto endpoint : left_table_ptr->node().endpoints())
    {
      std::cout << "left node " << left_table_ptr->node().type() << " has endpoint " << endpoint.second->field << ", type "
                << endpoint.second->type_name;
      auto sptr = endpoint.second->foreign_endpoint.lock();
      if (sptr)
        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
      else
        std::cout << " (no foreign endpoint)\n";
    }

    auto right_table_ptr = right_table_.lock();
    for(auto endpoint : right_table_ptr->node().endpoints())
    {
      std::cout << "right node " << right_table_ptr->node().type() << " has endpoint " << endpoint.second->field << ", type "
                << endpoint.second->type_name;
      auto sptr = endpoint.second->foreign_endpoint.lock();
      if (sptr)
        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
      else
        std::cout << " (no foreign endpoint)\n";
    }

    left_endpoint_ = table_.node().find_endpoint(std::type_index(typeid(typename table_type::left_value_type)))->second;
    std::cout << *left_endpoint_ << "\n";
    auto right_endpoint_iterator = table_.node().find_endpoint(std::type_index(typeid(typename table_type::right_value_type)));

    if (right_endpoint_iterator != table_.node().endpoint_end()) {
      right_endpoint_ = right_endpoint_iterator->second;
      std::cout << *right_endpoint_ << "\n";
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
  void serialize(const char *, V &x)
  {
    // must be right side value
    // if left table is loaded
    // insert it into concrete object
    // else
    // insert into relation data
    if (left_table_ptr_->is_loaded()) {
      has_many_item_holder<V> value(x, nullptr);
      left_endpoint_->insert_value_into_foreign(value, left_proxy_);
    } else {
      // Todo: append to left tables relation data
//      left_table_ptr_->append_relation_data(left_proxy_, x);
    }
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
  void serialize(const char *, belongs_to<V> &x, cascade_type cascade)
  {
    // check wether is left or right side value
    // left side will be determined first
    std::shared_ptr<basic_identifier> pk = x.primary_key();
    if (!pk) {
      return;
    }

    std::cout << "belongs_to pk is " << *pk << "\n";

    if (left_proxy_ == nullptr) {
//    if (left_table_ptr_->node().type_index() == std::type_index(typeid(V))) {
      std::cout << "belongs_to " << left_table_ptr_->node().type() << " is left (loaded: " << left_table_ptr_->is_loaded() << ")\n";
      // if left is not loaded
      left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);

    } else {
      std::cout << "belongs_to " << right_table_ptr_->node().type() << " is right (loaded: " << right_table_ptr_->is_loaded() << ")\n";

      object_proxy* right_proxy = acquire_proxy(x, pk, cascade, right_table_ptr_);
      if (left_table_ptr_->is_loaded()) {
        left_endpoint_->insert_value_into_foreign(right_proxy, left_proxy_);
      } else {
        // Todo: append to left tables relation data
        left_table_ptr_->append_relation_data(table_.name(), left_proxy_->pk(), right_proxy);
      }

      if (right_table_ptr_->is_loaded()) {
        right_endpoint_->insert_value_into_foreign(left_proxy_, right_proxy);
      } else {
        // Todo: append to left tables relation data
        right_table_ptr_->append_relation_data(table_.name(), right_proxy->pk(), left_proxy_);
      }

    }
  }

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
  void serialize(const char *id, basic_has_many<V, C> &, const char *, const char *) { }

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

/// @endcond
}

}
#endif //OOS_RELATION_RESOLVER_HPP
