#ifndef OOS_BASIC_TABLE_HPP
#define OOS_BASIC_TABLE_HPP

#include "matador/orm/export.hpp"

#include "matador/object/identifier_proxy_map.hpp"

#include "matador/orm/basic_relation_data.hpp"

#include <string>
#include <functional>

namespace matador {

/// @cond MATADOR_DEV

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

namespace detail {
class object_proxy_info
{
public:
  object_proxy_info() = default;
  explicit object_proxy_info(object_proxy *p) : proxy(p) {}
  object_proxy_info(const object_proxy_info &) = default;
  object_proxy_info(object_proxy_info &&) = default;
  object_proxy_info& operator=(const object_proxy_info &) = default;
  object_proxy_info& operator=(object_proxy_info &&) = default;
  ~object_proxy_info() = default;

  object_proxy *proxy;
  std::vector<basic_identifier*> primary_keys;
};

typedef std::unordered_map<basic_identifier*, object_proxy_info, detail::identifier_hash<basic_identifier>, detail::identifier_equal> t_identifier_proxy_infos;
}

/// @endcond

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
  typedef std::unordered_map<std::string, std::shared_ptr<detail::basic_relation_data>> t_relation_data_map; /**< Shortcut to unordered relation data map */

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

  /**
   * @brief Returns the underlaying prototype node
   *
   * @return The underlaying prototype node
   */
  prototype_node& node();

  /**
   * @brief Returns the underlaying prototype node
   *
   * @return The underlaying prototype node
   */
  const prototype_node& node() const;

  /**
   * @brief Marks the table as not loaded
   */
  virtual void reset();

  /**
   * @brief Access the underlaying persistence unit object
   *
   * @return The underlaying persistence unit object
   */
  persistence& persistence_unit();

  /**
   * @brief Access the underlaying persistence unit object
   *
   * @return The underlaying persistence unit object
   */
  const persistence& persistence_unit() const;

  /// @cond MATADOR_DEV

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

  detail::t_identifier_proxy_infos::iterator insert_proxy(basic_identifier *pk, object_proxy *proxy);
  detail::t_identifier_proxy_infos::iterator find_proxy(basic_identifier *pk);
  detail::t_identifier_proxy_infos::iterator begin_proxy();
  detail::t_identifier_proxy_infos::iterator end_proxy();

  t_relation_data_map::iterator find_relation_data(const char *id);
  t_relation_data_map::iterator begin_relation_data();
  t_relation_data_map::iterator end_relation_data();

protected:
  persistence &persistence_;

  detail::t_identifier_proxy_infos identifier_proxy_map_;

  t_relation_item_map has_many_relations_;

  t_relation_data_map relation_data_map_;

  bool is_loaded_ = false;

  prototype_node &node_;

  /// @endcond
};

}

#endif //OOS_BASIC_TABLE_HPP
