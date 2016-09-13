//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "object/object_proxy.hpp"
#include "object/object_store.hpp"

#include "orm/basic_table.hpp"
#include "orm/identifier_binder.hpp"
#include "orm/identifier_column_resolver.hpp"
#include "orm/relation_resolver.hpp"
#include "orm/relation_item_appender.hpp"

#include "sql/query.hpp"

namespace oos {

class connection;

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
class table : public basic_table
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
  table(prototype_node *node, persistence &p)
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

      detail::t_identifier_map::iterator i = identifier_proxy_map_.find(id);
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
    insert_.bind((T*)proxy->obj(), 0);
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    T *obj = (T*)proxy->obj();
    size_t pos = update_.bind(obj, 0);
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
    column id = detail::identifier_column_resolver::resolve<T>();
    update_ = q.update().set().where(id == 1).prepare(conn);
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

}

#endif //OOS_TABLE_HPP
