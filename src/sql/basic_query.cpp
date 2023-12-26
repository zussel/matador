#include "matador/sql/columns.hpp"
#include "matador/sql/basic_query.hpp"
#include "matador/sql/connection.hpp"

namespace matador {

namespace detail {

basic_query::basic_query()
  : state(QUERY_BEGIN)
  , update_columns_(new columns(columns::WITHOUT_BRACKETS))
  , query_value_column_processor_(update_columns_, rowvalues_)
{}

void basic_query::reset_query(t_query_command query_command)
{
  sql_.reset(query_command);
  sql_.table_name("");
  state = QUERY_BEGIN;
  update_columns_->columns_.clear();
}

std::string basic_query::str(bool prepared)
{
  return str(conn_, prepared);
}

std::string basic_query::str(connection &conn, bool prepared)
{
  if (prepared) {
    return conn.dialect()->prepare(sql_).sql;
  } else {
    return conn.dialect()->direct(sql_);
  }
}

const sql& basic_query::stmt() const
{
  return sql_;
}

std::string basic_query::tablename() const
{
  return sql_.table_name();
}

void basic_query::throw_invalid(state_t next, state_t current)
{
  std::stringstream msg;
  switch (next) {
    case QUERY_CREATE:
    case QUERY_DROP:
    case QUERY_SELECT:
    case QUERY_INSERT:
    case QUERY_UPDATE:
    case QUERY_DELETE:
      if (current != QUERY_BEGIN) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_VALUES:
      if (current != QUERY_INSERT) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_WHERE:
    case QUERY_COND_WHERE:
      if (current != QUERY_SELECT &&
          current != QUERY_COLUMN &&
          current != QUERY_UPDATE &&
          current != QUERY_SET &&
          current != QUERY_DELETE &&
          current != QUERY_FROM &&
          current != QUERY_COND_WHERE)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_COLUMN:
      if (current != QUERY_SELECT &&
          current != QUERY_COLUMN)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_FROM:
      if (current != QUERY_SELECT) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_SET:
      if (current != QUERY_UPDATE &&
          current != QUERY_SET)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_ORDERBY:
      if (current != QUERY_SELECT &&
          current != QUERY_WHERE &&
          current != QUERY_FROM &&
          current != QUERY_COND_WHERE)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_ORDER_DIRECTION:
      if (current != QUERY_ORDERBY) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    default:
      throw std::logic_error("unknown state");
  }
}

std::string basic_query::state2text(basic_query::state_t state)
{
  switch (state) {
    case QUERY_BEGIN:
      return "begin";
    case QUERY_CREATE:
      return "create";
    case QUERY_DROP:
      return "drop";
    case QUERY_SELECT:
      return "select";
    case QUERY_INSERT:
      return "insert";
    case QUERY_UPDATE:
      return "update";
    case QUERY_DELETE:
      return "delete";
    case QUERY_COLUMN:
      return "column";
    case QUERY_SET:
      return "set";
    case QUERY_FROM:
      return "from";
    case QUERY_WHERE:
      return "where";
    case QUERY_COND_WHERE:
      return "condition_where";
    case QUERY_ORDERBY:
      return "order_by";
    case QUERY_ORDER_DIRECTION:
      return "order_direction";
    case QUERY_GROUPBY:
      return "group_by";
    default:
      return "unknown";
  }
}

//std::unordered_map<std::type_index, std::string> basic_query::tablename_map_ = std::unordered_map<std::type_index, std::string>();

}

}