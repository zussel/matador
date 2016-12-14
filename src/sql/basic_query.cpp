//
// Created by sascha on 24.05.16.
//

#include "oos/sql/column.hpp"
#include "oos/sql/basic_query.hpp"
#include "oos/sql/connection.hpp"

namespace oos {

namespace detail {

basic_query::basic_query(const std::string &table_name)
  : state(QUERY_BEGIN)
  , table_name_(table_name)
  , update_columns_(new columns(columns::WITHOUT_BRACKETS))
  , query_value_column_processor_(update_columns_, rowvalues_)
{}

basic_query::basic_query(const connection &conn, const std::string &table_name)
  : state(QUERY_BEGIN)
  , table_name_(table_name)
  , update_columns_(new columns(columns::WITHOUT_BRACKETS))
  , query_value_column_processor_(update_columns_, rowvalues_)
  , conn_(conn)
{}

void basic_query::reset_query(t_query_command query_command)
{
  sql_.reset(query_command);
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
    return conn.dialect()->prepare(sql_);
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
  return table_name_;
}

void basic_query::throw_invalid(state_t next, state_t current)
{
  std::stringstream msg;
  switch (next) {
    case basic_query::QUERY_CREATE:
    case basic_query::QUERY_DROP:
    case basic_query::QUERY_SELECT:
    case basic_query::QUERY_INSERT:
    case basic_query::QUERY_UPDATE:
    case basic_query::QUERY_DELETE:
      if (current != QUERY_BEGIN) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_WHERE:
    case basic_query::QUERY_COND_WHERE:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_COLUMN &&
          current != basic_query::QUERY_UPDATE &&
          current != basic_query::QUERY_SET &&
          current != basic_query::QUERY_DELETE &&
          current != basic_query::QUERY_FROM &&
          current != basic_query::QUERY_COND_WHERE)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_COLUMN:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_COLUMN)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_FROM:
      if (current != basic_query::QUERY_SELECT) {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_SET:
      if (current != basic_query::QUERY_UPDATE &&
          current != basic_query::QUERY_SET)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_ORDERBY:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_WHERE &&
          current != basic_query::QUERY_FROM &&
          current != basic_query::QUERY_COND_WHERE)
      {
        msg << "invalid next state: [" << state2text(next) << "] (current: " << state2text(current) << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_ORDER_DIRECTION:
      if (current != basic_query::QUERY_ORDERBY) {
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

std::unordered_map<std::type_index, std::string> basic_query::tablename_map_ = {};

}

}