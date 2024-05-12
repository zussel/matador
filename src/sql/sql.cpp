#include "matador/sql/sql_context.hpp"
#include "matador/sql/basic_dialect.hpp"

#include <iostream>

namespace matador {

sql_context::sql_context()
  : command_type_(t_query_command::UNKNOWN)
{ }

sql_context::~sql_context()
{
  reset(t_query_command::UNKNOWN);
}

void sql_context::append(const std::shared_ptr<detail::token> &token_ptr)
{
  token_list_.push_back(token_ptr);
}

void sql_context::append(const sql_context &stmt)
{
  append(std::make_shared<detail::query>(stmt));
}

void sql_context::reset(t_query_command command_type)
{
  command_type_ = command_type;
  token_list_.clear();
}

std::string sql_context::command() const
{
  switch (command_type_) {
    case t_query_command::CREATE:
      return "create";
    case t_query_command::DROP:
      return "drop";
    case t_query_command::INSERT:
      return "insert";
    case t_query_command::UPDATE:
      return "update";
    case t_query_command::REMOVE:
      return "delete";
    case t_query_command::SELECT:
      return "select";
    case t_query_command::UNKNOWN:
    default:
      return "unknown";
  }
}

std::string sql_context::table_name() const
{
  return table_name_;
}

void sql_context::table_name(const std::string &tname)
{
  table_name_ = tname;
}

namespace detail {
query::query(const sql_context &s)
  : token(NONE), sql_(s)
{ }

void query::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

}
}
