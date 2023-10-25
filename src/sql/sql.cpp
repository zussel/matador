#include "matador/sql/sql.hpp"
#include "matador/sql/basic_dialect.hpp"

#include <iostream>

namespace matador {

sql::sql(const sql &x)
: command_type_(x.command_type_)
, table_name_(x.table_name_)
{
}

sql &sql::operator=(const sql &x)
{
  return *this;
}

sql::sql(sql &&x) noexcept
: command_type_(x.command_type_)
, token_list_(std::move(x.token_list_))
, table_name_(std::move(x.table_name_))
{

}

sql &sql::operator=(sql &&x) noexcept
{
  return *this;
}

void sql::append(std::unique_ptr<detail::token> token_ptr)
{
  token_list_.push_back(std::move(token_ptr));
}

void sql::append(const sql &stmt)
{
  append(std::make_unique<detail::query>(stmt));
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
