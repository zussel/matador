#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#include "matador/sql/commands.hpp"
#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/connection.hpp"

#include "matador/utils/any.hpp"
#include "query_value_column_processor.hpp"
#include "query_value_creator.hpp"

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
  basic_query();

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
    QUERY_WHERE,
    QUERY_VALUES,
    QUERY_COND_WHERE,
    QUERY_ORDERBY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUPBY
  };

  /// @endcond

protected:

  /// @cond MATADOR_DEV

  static void throw_invalid(state_t next, state_t current);

  static std::string state2text(state_t state);

  /// @endcond

protected:
  /// @cond MATADOR_DEV
  sql sql_;
  state_t state;
  std::shared_ptr<columns> update_columns_;
  std::vector<matador::any> rowvalues_;
  detail::query_value_column_processor query_value_column_processor_;
  detail::query_value_creator query_value_creator_;
  connection conn_;
  /// @endcond
};

}

}
#endif //OOS_BASIC_QUERY_HPP
