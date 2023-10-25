#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#include "matador/sql/commands.hpp"
#include "matador/sql/sql.hpp"

#include <string>
#include <unordered_set>
#include <unordered_map>

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

  enum class state_t {
    QUERY_INIT,
    QUERY_CREATE,
    QUERY_TABLE,
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
    QUERY_VALUES,
    QUERY_COND_WHERE,
    QUERY_ORDER_BY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUP_BY,
    QUERY_LIMIT,
    QUERY_FINISH
  };

  /// @endcond

protected:

  /// @cond MATADOR_DEV

  void transition_to(state_t next);

  /// @endcond

protected:
  /// @cond MATADOR_DEV
  sql sql_;
  state_t state{state_t::QUERY_INIT};

  using query_state_set = std::unordered_set<state_t>;
  using query_state_transition_map = std::unordered_map<state_t, query_state_set>;
  using query_state_to_string_map = std::unordered_map<state_t, std::string>;

  static query_state_transition_map transitions_;
  static query_state_to_string_map state_strings_;
  /// @endcond
};

}

}
#endif //OOS_BASIC_QUERY_HPP
