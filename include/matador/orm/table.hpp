//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/prototype_node.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/orm/basic_table.hpp"
#include "matador/orm/identifier_binder.hpp"
#include "matador/orm/identifier_column_resolver.hpp"
#include "matador/orm/relation_resolver.hpp"
#include "matador/orm/relation_item_appender.hpp"

#include "matador/sql/query.hpp"

namespace matador {

class connection;

template < class T, class Enabled = void >
class table;

class basic_has_many_to_many_item;

/**
 * @brief Represents a database table
 *
 * This class represents one table on the database
 * created by the given type.
 * All columns are derived from the given type.
 *
 * @tparam T The type of the table
 */
template < class T >
class table<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type> : public basic_table
{
public:
  /**
   * @brief Creates a new table
   *
   * Creates a new table for the given node and the
   * given persistence object.
   *
   * @param node The underlying prototype_node
   * @param p The underlying persistence object
   */
  table(prototype_node &node, persistence &p)
    : basic_table(node, p)
    , resolver_(*this)
  { }

  ~table() override = default;

  void create(connection &conn) override
  {
    query<T> stmt(name());
    stmt.create().execute(conn);
  }

  void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  void load(object_store &store) override
  {
    auto result = select_.execute();

    auto first = result.begin();
    auto last = result.end();

    while (first != last) {
      // try to find object proxy by id
      std::shared_ptr<basic_identifier> id(identifier_resolver_.resolve_object(first.get()));

      auto i = identifier_proxy_map_.find(id);
      if (i != identifier_proxy_map_.end()) {
        // use proxy;
        proxy_.reset(i->second);
        proxy_->reset(first.release(), false);
        identifier_proxy_map_.erase(i);
      } else {
        // create new proxy
        proxy_.reset(new object_proxy(first.release()));
      }

      ++first;
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);
    }

    // mark table as loaded
    is_loaded_ = true;
  }

  void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<T*>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  void update(object_proxy *proxy) override
  {
    T *obj = static_cast<T*>(proxy->obj());
    size_t pos = update_.bind(0, obj);
    binder_.bind(obj, &update_, pos);
    // Todo: check result
    update_.execute();
  }

  void remove(object_proxy *proxy) override
  {
    binder_.bind((T*)proxy->obj(), &delete_, 0);
    // Todo: check result
    delete_.execute();
  }

protected:
  /**
   * @brief Prepares the table object
   *
   * Prepares the table object for the given connection.
   * Subsequently some prepared statements are created:
   * - select
   * - insert
   * - update
   * - delete
   *
   * These statements will be used on the provide
   * methods.
   *
   * @param conn The database connection
   */
  void prepare(connection &conn) override
  {
    query<T> q(name());
    insert_ = q.insert().prepare(conn);
//    std::cout << "INSERT: " << insert_.str() << "\n";
    column id = detail::identifier_column_resolver::resolve<T>();
    update_ = q.update().where(id == 1).prepare(conn);
//    std::cout << "UPDATE: " << update_.str() << "\n";
    delete_ = q.remove().where(id == 1).prepare(conn);
    select_ = q.select().prepare(conn);
  }

  /**
   * @brief Append elements to a has many relation
   *
   * Append elements to a has many relation identified by id
   *
   * @param id The identifier of the has many relation object in entity
   * @param identifier_proxy_map The map with all relationship holding objects
   * @param has_many_relations The relationship elements
   */
//  void append_relation_items(const std::string &id, detail::t_identifier_map &identifier_proxy_map, basic_table::t_relation_item_map &has_many_relations) override
//  {
//    appender_.append(id, identifier_proxy_map, &has_many_relations);
//  }

private:
  detail::identifier_binder<T> binder_;

  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;
  statement<T> select_;

  detail::relation_resolver<T> resolver_;
//  detail::relation_item_appender<T> appender_;

  std::unique_ptr<object_proxy> proxy_;

  identifier_resolver<T> identifier_resolver_;
};

template < class T >
class table<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value>::type>
  : public basic_table, public detail::object_proxy_accessor
{
public:
  typedef T table_type;

  table(prototype_node &node, persistence &p)
  : basic_table(node, p)
  , resolver_(*this)
  {

    if (!node.is_relation_node()) {
      throw_object_exception("node " << node.type() << " isn't a relation node");
    }
  }

  void create(connection &conn) override
  {
    query<T> stmt(name());
    stmt.create(*this->node_.prototype<T>()).execute(conn);
  }

  void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  void prepare(connection &conn) override
  {
    query<T> q(name());

//    select_all_ = q.select().prepare(conn);

    T *proto = this->node_.prototype<T>();
    select_all_ = q.select({proto->left_column(), proto->right_column()}).prepare(conn);
    insert_ = q.insert(*proto).prepare(conn);

    column owner_id(proto->left_column());
    column item_id(proto->right_column());

    update_ = q.update(*proto).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);


    auto left_node = node_.tree()->find<typename table_type::left_value_type>();
    auto right_node = node_.tree()->find<typename table_type::right_value_type>();

//    for(auto endpoint : node_.endpoints()) {
//      std::cout << "node " << node_.type() << " has endpoint " << endpoint.second->field << ", type " << endpoint.second->type_name;
//      auto sptr = endpoint.second->foreign_endpoint.lock();
//      if (sptr)
//        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
//      else
//        std::cout << " (no foreign endpoint)\n";
//    }

    // find left table
//    std::cout << "preparing table " << node_.type() << " left table " << left_node->type() << " (field: " << proto->left_column() << ")\n";
    auto tid = find_table(left_node->type());
    if (tid == end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + std::string(left_node->type()) + " found");
//      std::cout << "no owner table " << std::string(left_node->type()) << " found\n";
    } else {
      left_table_ = tid->second;
    }

    // find right table
//    std::cout << "preparing table " << node_.type() << " right table " << right_node->type() << " (field: " << proto->right_column() << ")\n";
    tid = find_table(right_node->type());
    if (tid == end_table()) {
      // Todo: introduce throw_orm_exception
//      throw std::logic_error("no owner table " + std::string(right_node->type()) + " found");
//      std::cout << "no owner table " << std::string(right_node->type()) << " found\n";
    } else {
      right_table_ = tid->second;
    }
  }

  void load(object_store &store) override
  {
    if (is_loaded_) {
      return;
    }
    auto res = select_all_.execute();

    // set explicit creator function
//    T item(item_);
    prototype_node &node = this->node_;

//    for (auto endpoint : node.endpoints()) {
//      std::cout << "node " << node.type() << " has endpoint " << endpoint.first.name()
//                << " (field: " << endpoint.second->field
//                << ", type: " << endpoint.second->type_name
//                << ", node: " << endpoint.second->node->type() << ")\n";
//    }
//    auto func = [&node]() {
//      return node.create<T>();
//    };

    res.creator([&node]() {
      return node.create<T>();
    });

    auto left_table_ptr = left_table_.lock();
    auto right_table_ptr = right_table_.lock();

    auto left_endpoint = node_.find_endpoint(std::type_index(typeid(typename table_type::left_value_type)))->second;
    auto right_endpoint_iterator = node_.find_endpoint(std::type_index(typeid(typename table_type::right_value_type)));

    std::shared_ptr<detail::basic_relation_endpoint> right_endpoint;
    if (right_endpoint_iterator != node_.endpoint_end()) {
      right_endpoint = right_endpoint_iterator->second;
    }

    auto first = res.begin();
    auto last = res.end();

    while (first != last) {
      // create new proxy of relation object
      proxy_.reset(new object_proxy(first.release()));
      ++first;
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);

      typename table_type::left_value_type lv;

      has_many_item_holder<typename table_type::left_value_type> holder;

      T *obj = proxy->obj<T>();

//      std::cout << "left object is loaded: " << obj->left().is_loaded() << " (type: "<< typeid(typename table_type::left_value_type).name() << ")\n";
//      std::cout << "right object is loaded: " << obj->right().is_loaded() << " (type: "<< typeid(typename table_type::right_value_type).name() << ")\n";

      resolve_object_links(obj->left(), obj->right(), left_endpoint, right_endpoint, left_table_ptr, right_table_ptr);

    }
//      auto i = owner_table_->has_many_relations_.find(relation_id_);
//      if (i == owner_table_->has_many_relations_.end()) {
//        i = owner_table_->has_many_relations_.insert(
//        std::make_pair(relation_id_, detail::t_identifier_multimap())).first;
//      }
//      i->second.insert(std::make_pair(proxy->obj<relation_type>()->owner(), proxy));
//    }

//    if (owner_table_->is_loaded()) {
//      // append items
//      owner_table_->append_relation_items(relation_id_, identifier_proxy_map_, owner_table_->has_many_relations_);
//    }

    is_loaded_ = true;
  }

  void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<T*>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  void update(object_proxy *proxy) override
  {
    size_t pos = update_.bind(0, static_cast<T*>(proxy->obj()));
    update_.bind(pos, static_cast<T*>(proxy->obj()));

    update_.execute();
  }

  void remove(object_proxy *proxy) override
  {
    delete_.bind(0, static_cast<T*>(proxy->obj()));
    delete_.execute();
  }

private:
  template < class Owner, class Value >
  void insert_has_many_relations(detail::t_identifier_multimap &id_map, const object_ptr<Owner> &owner, const object_ptr<Value> &value)
  {
    id_map.insert(std::make_pair(owner.primary_key(), std::make_unique<has_many_item_holder<Value>>(value, nullptr)));
  }

  template < class Owner, class Value >
  void insert_has_many_relations(detail::t_identifier_multimap &id_map, const object_ptr<Owner> &owner, const Value &value)
  {
    id_map.insert(std::make_pair(owner.primary_key(), std::make_unique<has_many_item_holder<Value>>(value, nullptr)));
//    id_map.insert(std::make_pair(owner.primary_key(), value));
  }

  template < class Owner, class Value >
  void insert_has_many_relations(detail::t_identifier_multimap &/*id_map*/, const Owner &/*owner*/, const object_ptr<Value> &/*value*/)
  {
//    id_map.insert(std::make_pair(owner.primary_key(), this->proxy(obj->left())));
  }

  template < class Owner, class Value >
  void insert_has_many_relations(detail::t_identifier_multimap &id_map, const Owner &owner, Value &value)
  {

  }

  template < class Owner, class Value >
  void insert_value_into_foreign_endpoint(const std::shared_ptr<detail::basic_relation_endpoint> &foreign_endpoint,
                                          const Owner &owner, const Value &value)
  {

  }

  template < class Owner, class Value >
  void insert_value_into_foreign_endpoint(const std::shared_ptr<detail::basic_relation_endpoint> &foreign_endpoint,
                                          const object_ptr<Owner> &owner, const Value &value)
  {
    foreign_endpoint->insert_value_into_foreign(
      has_many_item_holder<Value>(value, nullptr),
      this->proxy(owner)
    );
  }

  template < class Owner, class Value >
  void insert_value_into_foreign_endpoint(const std::shared_ptr<detail::basic_relation_endpoint> &foreign_endpoint,
                                          const Owner &owner, const object_ptr<Value> &value)
  {
    foreign_endpoint->insert_value_into_foreign(
      has_many_item_holder<typename table_type::right_value_type>(owner, nullptr),
      this->proxy(value)
    );
  }

  template < class Owner, class Value >
  void insert_value_into_foreign_endpoint(const std::shared_ptr<detail::basic_relation_endpoint> &foreign_endpoint,
                                          const object_ptr<Owner> &owner, const object_ptr<Value> &value)
  {
    foreign_endpoint->insert_value_into_foreign(
      has_many_item_holder<Owner>(this->proxy(owner), nullptr),
      this->proxy(value)
    );
  }

  template < class Owner, class Value >
  void resolve_object_links(const object_ptr<Owner> &owner, const object_ptr<Value> &value,
                            const std::shared_ptr<detail::basic_relation_endpoint> &left_endpoint,
                            const std::shared_ptr<detail::basic_relation_endpoint> &right_endpoint,
                            const std::shared_ptr<basic_table> &left_table_ptr,
                            const std::shared_ptr<basic_table> &right_table_ptr
  )
  {
    if (owner.is_loaded() && value.is_loaded()) {
      // both types left and right are loaded
      if (left_endpoint->type == detail::basic_relation_endpoint::BELONGS_TO) {
        insert_value_into_foreign_endpoint(left_endpoint, value, owner);
      }
      if (right_endpoint && right_endpoint->type == detail::basic_relation_endpoint::BELONGS_TO) {
        insert_value_into_foreign_endpoint(right_endpoint, owner, value);
      }
    } else if (owner.is_loaded()) {
      // left is loaded right isn't loaded
      if (left_endpoint->type == detail::basic_relation_endpoint::BELONGS_TO) {
        insert_value_into_foreign_endpoint(left_endpoint, owner, owner);
      }
      if (right_table_ptr) {
        prepare_foreign_table_objects(value, owner, right_endpoint, right_table_ptr);
      }
    } else if (value.is_loaded()) {
      // right is loaded left isn't loaded
      if (right_endpoint && right_endpoint->type == detail::basic_relation_endpoint::BELONGS_TO) {
        insert_value_into_foreign_endpoint(right_endpoint, owner, value);
      }
      if (left_table_ptr) {
        prepare_foreign_table_objects(owner, value, left_endpoint, left_table_ptr);
      }
    } else {
      // none is loaded
      if (left_table_ptr) {
        prepare_foreign_table_objects(owner, value, left_endpoint, left_table_ptr);
      }
      if (right_table_ptr) {
        prepare_foreign_table_objects(value, owner, right_endpoint, right_table_ptr);
      }
    }
  }

  template < class Owner, class Value >
  void resolve_object_links(const object_ptr<Owner> &owner, const Value &value,
                            const std::shared_ptr<detail::basic_relation_endpoint> &left_endpoint,
                            const std::shared_ptr<detail::basic_relation_endpoint> &/*right_endpoint*/,
                            const std::shared_ptr<basic_table> &left_table_ptr,
                            const std::shared_ptr<basic_table> &/*right_table_ptr*/)
  {
    if (owner.is_loaded()) {
      // left is loaded right isn't loaded
      if (left_endpoint->type == detail::basic_relation_endpoint::BELONGS_TO) {
        insert_value_into_foreign_endpoint(left_endpoint, owner, value);
      }
    } else {
      if (left_table_ptr) {
        prepare_foreign_table_objects(owner, value, left_endpoint, left_table_ptr);
      }
//    }
//    if (right_table_ptr) {
//      prepare_foreign_table_objects(value, owner, right_endpoint, right_table_ptr);
    }
  }

  template < class Owner, class Value >
  void prepare_foreign_table_objects(const object_ptr<Owner> &owner, const object_ptr<Value> &value,
                                     const std::shared_ptr<detail::basic_relation_endpoint> &relation_endpoint,
                                     const std::shared_ptr<basic_table> &table_ptr)
  {
//        std::cout << "left endpoint field " << relation_endpoint->field << " (table: " << table_ptr->node_.type() << ")\n";
    auto foreign_endpoint = relation_endpoint->foreign_endpoint.lock();
    if (foreign_endpoint) {
//          std::cout << "foreign left endpoint field " << foreign_endpoint->field << " (type: " << foreign_endpoint->type_name << ")\n";
      auto r = table_ptr->has_many_relations_.find(foreign_endpoint->field);
      if (r == table_ptr->has_many_relations_.end()) {
//            std::cout << "inserting new multimap for field " << foreign_endpoint->field << "\n";
        r = table_ptr->has_many_relations_.insert(
            std::make_pair(foreign_endpoint->field, detail::t_identifier_multimap())
        ).first;
      }
//            std::cout << "adding to existing multimap for field " << foreign_endpoint->field << "\n";
      insert_has_many_relations(r->second, owner, value);
//          r->second.insert(std::make_pair(obj->left().primary_key(), this->proxy(obj->right())));
    }
  }

  template < class Owner, class Value >
  void prepare_foreign_table_objects(const object_ptr<Owner> &owner, const Value &value,
                                     const std::shared_ptr<detail::basic_relation_endpoint> &relation_endpoint,
                                     const std::shared_ptr<basic_table> &table_ptr)
  {
//        std::cout << "left endpoint field " << relation_endpoint->field << " (table: " << table_ptr->node_.type() << ")\n";
    auto left_foreign_endpoint = relation_endpoint->foreign_endpoint.lock();
    if (left_foreign_endpoint) {
//          std::cout << "foreign left endpoint field " << left_foreign_endpoint->field << " (type: " << left_foreign_endpoint->type_name << ")\n";
      auto r = table_ptr->has_many_relations_.find(left_foreign_endpoint->field);
      if (r == table_ptr->has_many_relations_.end()) {
//            std::cout << "inserting new multimap for field " << left_foreign_endpoint->field << "\n";
        r = table_ptr->has_many_relations_.insert(
            std::make_pair(left_foreign_endpoint->field, detail::t_identifier_multimap())
        ).first;
      }
//            std::cout << "adding to existing multimap for field " << left_foreign_endpoint->field << "\n";
      insert_has_many_relations(r->second, owner, value);
//          r->second.insert(std::make_pair(obj->left().primary_key(), this->proxy(obj->right())));
    }
  }
private:
  statement<T> select_all_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;

  std::weak_ptr<basic_table> left_table_;
  std::weak_ptr<basic_table> right_table_;
};

}

#endif //OOS_TABLE_HPP
