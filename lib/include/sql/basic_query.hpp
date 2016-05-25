//
// Created by sascha on 24.05.16.
//

#ifndef OOS_BASIC_QUERY_HPP
#define OOS_BASIC_QUERY_HPP

#include "sql/commands.hpp"
#include "sql/sql.hpp"
#include "sql/column.hpp"

#include <string>

namespace oos {

class connection;

namespace detail {

class basic_query
{
public:
  basic_query(const std::string &table_name);

  /**
   * Resets the query.
   *
   * @return A reference to the query.
   */
  void reset_query(t_query_command query_command);

  std::string str(connection &conn, bool prepared);


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
    QUERY_GROUPBY,
    QUERY_EXECUTED,
    QUERY_PREPARED,
    QUERY_BOUND
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
