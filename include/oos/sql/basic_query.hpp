//
// Created by sascha on 24.05.16.
//

#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#ifdef _MSC_VER
#ifdef oos_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "oos/sql/commands.hpp"
#include "oos/sql/sql.hpp"
#include "oos/sql/column.hpp"
#include "oos/sql/connection.hpp"

#include "oos/utils/any.hpp"
#include "query_value_column_processor.hpp"
#include "query_value_creator.hpp"

#include <string>

namespace oos {

class connection;

namespace detail {

/**
 * @brief Base class for all query implementations
 *
 * This class acts as a base class for all types
 * of query implementations
 */
class OOS_SQL_API basic_query
{
public:
  /**
   * Creates a basic query for the given
   * table identified by table_name
   *
   * @param table_name The name of the table
   */
  basic_query(const std::string &table_name);

  /**
   * Creates a basic query for the given
   * table identified by table_name and the
   * given connection
   *
   * @param conn The default connection for the query
   * @param table_name The name of the table
   */
  basic_query(const connection &conn, const std::string &table_name);

  /**
   * Resets the query.
   *
   * @param query_command The query command to reset to
   */
  void reset_query(t_query_command query_command);

  /**
   * Get the query string build for internal
   * connection as prepared or direct statement
   *
   * @param prepared Indicates wether the query should
   *                 be interpreted as prepared or direct
   * @return The build query string
   */
  std::string str(bool prepared);

  /**
   * Get the query string build for the specific
   * connection as prepared or direct statement
   *
   * @param conn The connection to build for
   * @param prepared Indicates wether the query should
   *                 be interpreted as prepared or direct
   * @return The build query string
   */
  std::string str(connection &conn, bool prepared);

  /**
   * Get the underlying sql object
   *
   * @return The underlying sql
   */
  const sql& stmt() const;

  /**
   * Return the current tablename of the query
   *
   * @return Current tablename of the query
   */
  std::string tablename() const;

protected:

  /// @cond OOS_DEV

  enum state_t {
    QUERY_BEGIN,
    QUERY_CREATE,
    QUERY_DROP,
    QUERY_SELECT,
    QUERY_INSERT,
    QUERY_UPDATE,
    QUERY_DELETE,
    QUERY_COLUMN,
    QUERY_SET,
    QUERY_FROM,
    QUERY_WHERE,
    QUERY_COND_WHERE,
    QUERY_ORDERBY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUPBY
  };

  /// @endcond

protected:

  /// @cond OOS_DEV

  static void throw_invalid(state_t next, state_t current);

  static std::string state2text(state_t state);

  template < class T >
  bool determine_tablename()
  {
    auto i = basic_query::tablename_map_.find(std::type_index(typeid(T)));
    if (i != basic_query::tablename_map_.end()) {
      table_name_ = i->second;
      return true;
    }
    return false;
  }
  /// @endcond

protected:
  /// @cond OOS_DEV
  sql sql_;
  state_t state;
  std::string table_name_;
  std::shared_ptr<columns> update_columns_;
  std::vector<oos::any> rowvalues_;
  detail::query_value_column_processor query_value_column_processor_;
  detail::query_value_creator query_value_creator_;
  connection conn_;
  static std::unordered_map<std::type_index, std::string> tablename_map_;
  /// @endcond
};

}

}
#endif //OOS_BASIC_QUERY_HPP
