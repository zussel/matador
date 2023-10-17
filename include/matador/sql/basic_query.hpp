#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#include "matador/sql/commands.hpp"
#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query_value_column_processor.hpp"
#include "matador/sql/query_value_creator.hpp"
#include "matador/sql/value_serializer.hpp"
#include "matador/sql/value_column_serializer.hpp"

#include "matador/utils/any.hpp"

#include <string>

namespace matador {

class connection;

namespace detail {

/**
 * @brief Base class for all query implementations
 *
 * This class acts as a base class for all types
 * of query implementations
 */
class basic_query
{
public:
  /**
   * Creates a basic query for the given
   * table identified by table_name
   *
   * @param table_name The name of the table
   */
//  explicit basic_query(std::string table_name);
  basic_query();

  /**
   * Resets the query.
   *
   * @param query_command The query command to reset to
   */
  void reset_query(t_query_command query_command);

  /**
   * Get the query string build for the specific
   * connection as prepared or direct statement
   *
   * @param conn The connection to build for
   * @param prepared Indicates whether the query should
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
   * Return the current table name of the query
   *
   * @return Current table name of the query
   */
  std::string table_name() const;

protected:

  /// @cond MATADOR_DEV

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
    QUERY_INTO,
    QUERY_WHERE,
    QUERY_COND_WHERE,
    QUERY_ORDER_BY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUP_BY
  };

  /// @endcond

protected:

  /// @cond MATADOR_DEV

  static void throw_invalid(state_t next, state_t current);

  static std::string state2text(state_t state);

//  template < class T >
//  bool determine_table_name()
//  {
//    auto i = basic_query::table_name_map_.find(std::type_index(typeid(T)));
//    if (i != basic_query::table_name_map_.end()) {
//      table_name_ = i->second;
//      return true;
//    }
//    return false;
//  }
  /// @endcond

protected:
  /// @cond MATADOR_DEV
  sql sql_;
  state_t state;
  std::shared_ptr<columns> update_columns_;
  std::vector<matador::any> row_values_;
  detail::query_value_column_processor query_value_column_processor_;
  detail::query_value_creator query_value_creator_;
  detail::value_serializer value_serializer_;
  detail::value_column_serializer value_column_serializer_;
//  static std::unordered_map<std::type_index, std::string> table_name_map_;
  /// @endcond
};

}

}
#endif //OOS_BASIC_QUERY_HPP
