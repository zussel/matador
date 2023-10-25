#include "matador/sql/basic_query.hpp"
#include "matador/sql/connection.hpp"

namespace matador {

namespace detail {

// poor mens state machine
// but does the job for query
basic_query::query_state_transition_map basic_query::transitions_{
{state_t::QUERY_INIT, {state_t::QUERY_CREATE, state_t::QUERY_DROP, state_t::QUERY_SELECT, state_t::QUERY_INSERT, state_t::QUERY_UPDATE, state_t::QUERY_DELETE}},
{state_t::QUERY_CREATE, {state_t::QUERY_TABLE}},
{state_t::QUERY_DROP, {state_t::QUERY_TABLE}},
{state_t::QUERY_SELECT, {state_t::QUERY_FROM}},
{state_t::QUERY_INSERT, {state_t::QUERY_INTO}},
{state_t::QUERY_UPDATE, {state_t::QUERY_SET}},
{state_t::QUERY_DELETE, {state_t::QUERY_FROM}},
{state_t::QUERY_TABLE, {state_t::QUERY_FINISH}},
{state_t::QUERY_FROM, {state_t::QUERY_WHERE, state_t::QUERY_FINISH}},
{state_t::QUERY_SET, {state_t::QUERY_WHERE, state_t::QUERY_FINISH}},
{state_t::QUERY_INTO, {state_t::QUERY_VALUES}},
{state_t::QUERY_WHERE, {state_t::QUERY_ORDER_BY, state_t::QUERY_GROUP_BY, state_t::QUERY_LIMIT, state_t::QUERY_FINISH}},
{state_t::QUERY_ORDER_BY, {state_t::QUERY_ORDER_DIRECTION}},
{state_t::QUERY_ORDER_DIRECTION, {state_t::QUERY_LIMIT, state_t::QUERY_FINISH}},
{state_t::QUERY_GROUP_BY, {state_t::QUERY_FINISH}},
{state_t::QUERY_LIMIT, {state_t::QUERY_FINISH}},
{state_t::QUERY_VALUES, {state_t::QUERY_FINISH}},
{state_t::QUERY_FINISH, {}},
};

basic_query::query_state_to_string_map basic_query::state_strings_ {
{ state_t::QUERY_INIT, "init" },
{ state_t::QUERY_CREATE, "create" },
{ state_t::QUERY_DROP, "drop" },
{ state_t::QUERY_SELECT, "select" },
{ state_t::QUERY_INSERT, "insert" },
{ state_t::QUERY_UPDATE, "update" },
{ state_t::QUERY_DELETE, "delete" },
{ state_t::QUERY_TABLE, "table" },
{ state_t::QUERY_FROM, "from" },
{ state_t::QUERY_SET, "set" },
{ state_t::QUERY_INTO, "into" },
{ state_t::QUERY_WHERE, "where" },
{ state_t::QUERY_ORDER_BY, "order_by" },
{ state_t::QUERY_GROUP_BY, "group_by" },
{ state_t::QUERY_LIMIT, "limit" },
{ state_t::QUERY_FINISH, "finish" },
};

basic_query::basic_query()
{}

void basic_query::reset_query(t_query_command query_command)
{
  sql_.reset(query_command);
  sql_.table_name("");
  state = state_t::QUERY_INIT;
}

std::string basic_query::str(connection &conn, bool prepared)
{
  if (prepared) {
    auto result = conn.dialect()->prepare(sql_);
    return std::get<0>(result);
  } else {
    return conn.dialect()->direct(sql_);
  }
}

const sql& basic_query::stmt() const
{
  return sql_;
}

std::string basic_query::table_name() const
{
  return sql_.table_name();
}

void basic_query::transition_to(basic_query::state_t next) {
  if (transitions_[state].count(next) == 0) {
    throw std::logic_error("invalid next state " + state_strings_[next]);
  }
  state = next;
}

}

}