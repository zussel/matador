//
// Created by sascha on 24.05.16.
//

#include "sql/column.hpp"
#include "sql/basic_query.hpp"

namespace oos {

namespace detail {

basic_query::basic_query(const std::string &table_name)
: state(QUERY_BEGIN)
, table_name_(table_name)
, update_columns_(new columns(columns::WITHOUT_BRACKETS))
{}

void basic_query::reset_query(t_query_command query_command)
{
  sql_.reset(query_command);
  state = QUERY_BEGIN;
  update_columns_->columns_.clear();
}

const sql& basic_query::stmt() const
{
  return sql_;
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
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_WHERE:
    case basic_query::QUERY_COND_WHERE:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_COLUMN &&
          current != basic_query::QUERY_UPDATE &&
          current != basic_query::QUERY_SET &&
          current != basic_query::QUERY_DELETE)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_COLUMN:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_COLUMN)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_FROM:
      if (current != basic_query::QUERY_COLUMN) {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_SET:
      if (current != basic_query::QUERY_UPDATE &&
          current != basic_query::QUERY_SET)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case basic_query::QUERY_ORDERBY:
      if (current != basic_query::QUERY_SELECT &&
          current != basic_query::QUERY_WHERE &&
          current != basic_query::QUERY_COND_WHERE)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case QUERY_ORDER_DIRECTION:
      if (current != basic_query::QUERY_ORDERBY) {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    default:
      throw std::logic_error("unknown state");
  }
}

}

}