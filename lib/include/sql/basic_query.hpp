//
// Created by sascha on 24.05.16.
//

#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/commands.hpp"
#include "sql/sql.hpp"
#include "sql/column.hpp"

#include <string>

namespace oos {

class connection;

namespace detail {

class OOS_API basic_query
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

protected:
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

protected:
  static void throw_invalid(state_t next, state_t current);

  static std::string state2text(state_t state);

protected:
  sql sql_;
  state_t state;
  std::string table_name_;
  std::shared_ptr<columns> update_columns_;
};

}

}
#endif //OOS_BASIC_QUERY_HPP
