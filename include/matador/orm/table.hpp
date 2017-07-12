//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "matador/object/object_proxy.hpp"
#include "matador/object/object_store.hpp"
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

  virtual ~table() {}

  virtual void create(connection &conn) override
  {
    query<T> stmt(name());
    stmt.create().execute(conn);
  }

  virtual void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void load(object_store &store) override
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

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<T*>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    T *obj = static_cast<T*>(proxy->obj());
    size_t pos = update_.bind(0, obj);
    binder_.bind(obj, &update_, pos);
    // Todo: check result
    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
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
  virtual void prepare(connection &conn) override
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
  virtual void append_relation_items(const std::string &id, detail::t_identifier_map &identifier_proxy_map, basic_table::t_relation_item_map &has_many_relations) override
  {
    appender_.append(id, identifier_proxy_map, &has_many_relations);
  }

private:
  detail::identifier_binder<T> binder_;

  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;
  statement<T> select_;

  detail::relation_resolver<T> resolver_;
  detail::relation_item_appender<T> appender_;

  std::unique_ptr<object_proxy> proxy_;

  identifier_resolver<T> identifier_resolver_;
};

template < class T >
class table<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value>::type> : public basic_table, public detail::object_proxy_accessor
{
public:
  table(prototype_node &node, persistence &p)
  : basic_table(node, p)
  , resolver_(*this)
  {

    if (!node.is_relation_node()) {
      throw_object_exception("node " << node.type() << " isn't a relation node");
    }
  }

  virtual void create(connection &conn) override
  {
    for(auto endpoint : node_.endpoints()) {
      std::cout << "node " << node_.type() << " has endpoint " << endpoint.second->field << ", type " << endpoint.second->type_name;
      auto sptr = endpoint.second->foreign_endpoint.lock();
      if (sptr)
        std::cout << " (foreign node: " << sptr->node->type() << ")\n";
      else
        std::cout << " (no foreign endpoint)\n";
    }
    query<T> stmt(name());
    stmt.create(*this->node_.prototype<T>()).execute(conn);
  }

  virtual void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void prepare(connection &conn) override
  {
    query<T> q(name());

    select_all_ = q.select().prepare(conn);
    T *proto = this->node_.prototype<T>();
//    select_all_ = q.select({owner_id_column_, item_id_column_}).prepare(conn);
    insert_ = q.insert(*proto).prepare(conn);

    column owner_id(proto->left_column());
    column item_id(proto->right_column());

    update_ = q.update(*proto).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);

    // find left table
    auto left_endpoint = this->node_.find_endpoint(proto->left_column());
    if (left_endpoint == this->node_.tree()->end()) {
      throw_object_exception("couldn't find relation left endpoint " << proto->left_column());
    }
    auto tid = find_table(left_endpoint.second->node.type());
//    if (tid == end_table()) {
//      // Todo: introduce throw_orm_exception
//      throw std::logic_error("no owner table " + owner_type_ + " found");
//    }
//    owner_table_ = tid->second;
  }

  virtual void load(object_store &store) override
  {
    if (is_loaded_) {
      return;
    }
    auto res = select_all_.execute();

    // set explicit creator function
//    T item(item_);
    prototype_node &node = this->node_;

    auto func = [&node]() {

      T *i = node.create<T>();
      return i;
    };

    res.creator(func);

    auto first = res.begin();
    auto last = res.end();

    while (first != last) {
      // create new proxy of relation object
      proxy_.reset(new object_proxy(first.release()));
      ++first;
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);

      // append item to owner table
//      auto i = owner_table_->has_many_relations_.find(relation_id_);
//      if (i == owner_table_->has_many_relations_.end()) {
//        i = owner_table_->has_many_relations_.insert(
//        std::make_pair(relation_id_, detail::t_identifier_multimap())).first;
//      }
//      i->second.insert(std::make_pair(proxy->obj<relation_type>()->owner(), proxy));
    }

//    if (owner_table_->is_loaded()) {
//      // append items
//      owner_table_->append_relation_items(relation_id_, identifier_proxy_map_, owner_table_->has_many_relations_);
//    }

    is_loaded_ = true;
  }

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<T*>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    size_t pos = update_.bind(0, static_cast<T*>(proxy->obj()));
    update_.bind(pos, static_cast<T*>(proxy->obj()));

    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
  {
    delete_.bind(0, static_cast<T*>(proxy->obj()));
    delete_.execute();
  }

private:
  statement<T> select_all_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;

  table_ptr left_table_;
  table_ptr right_table_;
};

}

#endif //OOS_TABLE_HPP
