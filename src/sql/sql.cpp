#include "matador/sql/sql.hpp"
#include "matador/sql/basic_dialect.hpp"

#include <iostream>

namespace matador {

void sql::append(const std::shared_ptr<detail::token> &token_ptr)
{
  token_list_.push_back(token_ptr);
}

void sql::append(const sql &stmt)
{
  append(std::make_shared<detail::query>(stmt));
}

void sql::reset(t_query_command command_type)
{
  command_type_ = command_type;
  token_list_.clear();
}

std::string sql::command() const
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

std::string sql::table_name() const
{
  return table_name_;
}

void sql::table_name(const std::string &tname)
{
  table_name_ = tname;
}

namespace detail {
query::query(const sql &s)
  : token(NONE), sql_(s)
{ }

void query::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

}
}
