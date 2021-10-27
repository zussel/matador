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

template<class T, class Enabled = void>
class table;

class basic_has_many_to_many_item;

}


namespace matador {

/**
 * @brief Represents a database table
 *
 * This class represents one table on the database
 * created by the given type.
 * All columns are derived from the given type.
 *
 * @tparam T The type of the table
 */
template<class T>
class table<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type>
  : public basic_table
{
public:
  typedef T table_type; /**< Shortcut to the type the table represents */

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
    : basic_table(node, p), resolver_(*this)
  {}

  ~table() override = default;

  /**
   * @brief Create the table on database
   *
   * Creates the database table for
   * the given connection
   * @param conn The database connection
   */
  void create(connection &conn) override
  {
    query<table_type> stmt(name());
    stmt.create().execute(conn);
  }

  /**
   * @brief Drops the table on database
   *
   * Drops the database table for
   * the given connection
   *
   * @param conn The database connection
   */
  void drop(connection &conn) override
  {
    query<table_type> stmt(name());
    stmt.drop().execute(conn);
  }

  /**
   * @brief Loads tables data into store
   *
   * Loads the data of the table into the
   * given object store. It is also ensured
   * that all relations are resolved or at
   * least prepared for later resolve.
   *
   * @param store The object store to load the data into
   */
  void load(object_store &store) override
  {
    auto result = select_.execute();

    for (auto entity : result) {
      // try to find object proxy by id
      basic_identifier* id(identifier_resolver_.resolve_object(entity.get()));

      auto i = identifier_proxy_map_.find(id);
      if (i != identifier_proxy_map_.end()) {
        // use proxy;
        proxy_.reset(i->second.proxy);
        proxy_->reset(entity.release(), true, true);
      } else {
        // create new proxy
        proxy_.reset(new object_proxy(entity.release()));
      }

      object_proxy *proxy = store.insert<table_type>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);

      if (i != identifier_proxy_map_.end()) {
        auto proxy_info = i->second;
        identifier_proxy_map_.erase(i);
        for (auto pk : proxy_info.primary_keys) {
          delete pk;
        }
      }
    }

    // mark table as loaded
    is_loaded_ = true;
  }

  /**
   * @brief Insert the object proxy into the table
   *
   * Insert the given object proxy into the database
   * table.
   *
   * @param proxy The object proxy to be inserted
   */
  void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<table_type *>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  /**
   * @brief Updates the object proxy on database
   *
   * Updates the given object proxy on
   * the database.
   *
   * @param proxy The object proxy to update
   */
  void update(object_proxy *proxy) override
  {
    update_.reset();
    auto it = persistence_.proxy_identifier_map_.find(proxy);
    if (it == persistence_.proxy_identifier_map_.end()) {
      throw_object_exception("couldn't find proxy " << proxy);
    }
    auto *obj = static_cast<table_type *>(proxy->obj());
    size_t pos = update_.bind(0, obj);
    binder_.bind(obj, &update_, pos, it->second.get());
    // Todo: check result
    update_.execute();
  }

  /**
   * @brief Deletes the object proxy from table
   *
   * The object represented by the object proxy
   * is deleted from the table
   *
   * @param proxy The object proxy to be deleted
   */
  void remove(object_proxy *proxy) override
  {
    delete_.reset();
    binder_.bind(static_cast<table_type *>(proxy->obj()), &delete_, 0, proxy->pk());
    // Todo: check result
    delete_.execute();
  }

  void reset() override
  {
    basic_table::reset();
    select_.reset();
  }

/// @cond MATADOR_DEV
  template<class V>
  void append_relation_data(const std::string &field, basic_identifier *id, const V &val, object_proxy *owner);
/// @endcond

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
    query<table_type> q(name());
    insert_ = q.insert().prepare(conn);
    column id = detail::identifier_column_resolver::resolve<T>();
    update_ = q.update().where(id == 1).prepare(conn);
    delete_ = q.remove().where(id == 1).prepare(conn);
    select_ = q.select().prepare(conn);

    resolver_.prepare();
  }

private:
  detail::identifier_binder<table_type> binder_;

  statement<table_type> insert_;
  statement<table_type> update_;
  statement<table_type> delete_;
  statement<table_type> select_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;

  identifier_resolver<T> identifier_resolver_;
};

/// @cond MATADOR_DEV

template < class T >
template < class V >
void table<T, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, T>::value>::type>::append_relation_data(
  const std::string &field,
  basic_identifier *id,
  const V &val,
  object_proxy *owner)
{
  auto i = relation_data_map_.find(field);
  if (i == relation_data_map_.end()) {
      auto value = std::make_shared<detail::relation_data<V>>();
      value->append_data(id, val, owner);
      relation_data_map_.insert(std::make_pair(field, value));
  } else {
      auto value = std::static_pointer_cast<detail::relation_data<V>>(i->second);
      value->append_data(id, val, owner);
  }
}

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
    query<table_type> stmt(name());
    stmt.create(*node().template prototype<table_type>()).execute(conn);
  }

  void drop(connection &conn) override
  {
    query<table_type> stmt(name());
    stmt.drop().execute(conn);
  }

  void prepare(connection &conn) override
  {
    query<table_type> q(name());

    table_type *proto = node().template prototype<table_type>();
    select_all_ = q.select({proto->left_column(), proto->right_column()}).prepare(conn);
    insert_ = q.insert(*proto).prepare(conn);

    column owner_id(proto->left_column());
    column item_id(proto->right_column());

    update_ = q.update(*proto).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);

    resolver_.prepare();
  }

  void load(object_store &store) override
  {
    if (is_loaded_) {
      return;
    }
    auto result = select_all_.execute();

    // set explicit creator function
    prototype_node &node = this->node();

    result.creator([&node]() {
      return node.create<T>();
    });

    for (auto entity : result) {
      // create new proxy of relation object
      proxy_.reset(new object_proxy(entity.release()));
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);
    }

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

  template < class V >
  void append_relation_data(const std::string &, basic_identifier*, const V &) { }

private:
  statement<T> select_all_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;
};

/// @endcond

}

#include "matador/orm/relation_resolver.tpp"

#endif //OOS_TABLE_HPP
