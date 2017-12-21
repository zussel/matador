//
// Created by sascha on 3/24/16.
//

#ifndef OOS_BASIC_TABLE_HPP
#define OOS_BASIC_TABLE_HPP

#ifdef _MSC_VER
#ifdef matador_orm_EXPORTS
#define OOS_ORM_API __declspec(dllexport)
#define EXPIMP_ORM_TEMPLATE
#else
#define OOS_ORM_API __declspec(dllimport)
#define EXPIMP_ORM_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_ORM_API
#endif

#include "matador/object/identifier_proxy_map.hpp"

#include "matador/orm/basic_relation_data.hpp"

#include <string>
#include <functional>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

//template < class T, class Enabled=void >
//class relation_resolver;

/// @endcond

}

class connection;
class object_proxy;
class object_store;
class persistence;

template < class T >
bool is_loaded(const T &)
{
  return true;
}

template <>
bool is_loaded(const object_holder &holder);

/**
 * @brief Base class for kind of tables
 *
 * This class acts as a base class for all kind
 * of tables (common table and relation table)
 */
class OOS_ORM_API basic_table
{
public:
  typedef std::shared_ptr<basic_table> table_ptr;                                             /**< Shortcut to table shared pointer */
  typedef std::unordered_map<std::string, table_ptr> t_table_map;                             /**< Shortcut to an unordered map of table shared pointer*/
  typedef std::unordered_map<std::string, detail::t_identifier_multimap> t_relation_item_map; /**< Shortcut to an unordered identifier multimap */

public:
  /**
   * @brief Creates a basic_table
   *
   * Creates a basic_table for the given prototype_node
   * and the persistence object
   *
   * @param node The underlying prototype_node
   * @param p The underlying persistence object
   */
  basic_table(prototype_node &node, persistence &p);

  virtual ~basic_table() = default;

  /**
   * @brief Returns the name of the table
   *
   * @return The name of the table
   */
  std::string name() const;

  /**
   * @brief Interface for creating a table
   *
   * Interface for creating a table for the
   * given database connection.
   *
   * @param conn The database connection
   */
  virtual void create(connection &conn) = 0;

  /**
   * @brief Interface for droping a table
   *
   * Interface for droping a table for the
   * given database connection.
   *
   * @param conn The database connection
   */
  virtual void drop(connection &conn) = 0;

  /**
   * @brief Interface for loading a table
   *
   * Interface for loading a table into the
   * given object_store.
   *
   * @param p The object_store to load the table into
   */
  virtual void load(object_store &p) = 0;

  /**
   * @brief Interface for inserting an object
   *
   * Interface for inserting an object represented
   * by the given object_proxy
   *
   * @param proxy The proxy representing the object to be inserted
   */
  virtual void insert(object_proxy *proxy) = 0;

  /**
   * @brief Interface for updating an object
   *
   * Interface for updating an object represented
   * by the given object_proxy
   *
   * @param proxy The proxy representing the object to be updated
   */
  virtual void update(object_proxy *proxy) = 0;

  /**
   * @brief Interface for deleting an object
   *
   * Interface for deleting an object represented
   * by the given object_proxy
   *
   * @param proxy The proxy representing the object to be deleted
   */
  virtual void remove(object_proxy *proxy) = 0;

  /**
   * @brief Returns true if the table is laready loaded
   *
   * @return True if the table is laready loaded
   */
  bool is_loaded() const;

  prototype_node& node();

  const prototype_node& node() const;

  void reset();

//  template < class T >
//  virtual void append_relation_data(const std::string &field, const std::shared_ptr<basic_identifier> &id, const T &data) = 0;
//  virtual void append_relation_data(const std::string &field, const std::shared_ptr<basic_identifier> &id, object_proxy *data) = 0;

protected:
  /// @cond MATADOR_DEV

//  template < class T, class Enabled >
//  friend class detail::relation_resolver;
//  friend class persistence;

public:
  t_table_map::iterator find_table(const std::string &type);
  template < class T >
  t_table_map::iterator find_table()
  {
    auto node = node_.tree()->find<T>();
    return find_table(node->type());
  }

  t_table_map::iterator begin_table();
  t_table_map::iterator end_table();

  virtual void prepare(connection &conn) = 0;

  persistence &persistence_;

  detail::t_identifier_map identifier_proxy_map_;

  t_relation_item_map has_many_relations_;

  typedef std::unordered_map<std::string, std::shared_ptr<detail::basic_relation_data>> t_relation_data_map;

  t_relation_data_map relation_data_map_;

  bool is_loaded_ = false;

  /// @endcond

private:
  prototype_node &node_;
};

}

#endif //OOS_BASIC_TABLE_HPP
