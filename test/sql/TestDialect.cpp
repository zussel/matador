//
// Created by sascha on 2/26/16.
//

#include "TestDialect.hpp"

#include "sql/token.hpp"

std::string TestDialect::compile(oos::detail::limit &limit)
{
  return "";
}

std::string TestDialect::compile(oos::detail::remove &remove1)
{
  return "";
}

std::string TestDialect::compile(oos::detail::basic_value &value)
{
  return "";
}

std::string TestDialect::compile(oos::detail::insert &insert)
{
  return "";
}

std::string TestDialect::compile(oos::detail::group_by &by)
{
  return "";
}

std::string TestDialect::compile(oos::detail::desc &desc)
{
  return "";
}

std::string TestDialect::compile(oos::detail::asc &asc)
{
  return "";
}

std::string TestDialect::compile(oos::detail::order_by &by)
{
  return "";
}

std::string TestDialect::compile(oos::detail::basic_condition &token)
{
  return "";
}

std::string TestDialect::compile(oos::detail::where &where)
{
  return "";
}

std::string TestDialect::compile(oos::detail::from &from)
{
  return "";
}

std::string TestDialect::compile(oos::detail::varchar_column &column)
{
  return "";
}

std::string TestDialect::compile(oos::detail::identifier_column &column)
{
  return "";
}

std::string TestDialect::compile(oos::detail::column &column)
{
  return "";
}

std::string TestDialect::compile(oos::detail::columns &columns)
{
  return "";
}

std::string TestDialect::compile(oos::detail::set &set)
{
  return "";
}

std::string TestDialect::compile(oos::detail::update &update)
{
  return "";
}

std::string TestDialect::compile(oos::detail::distinct &distinct)
{
  return "";
}

std::string TestDialect::compile(oos::detail::select &select1)
{
  return token(select1.type) + " ";
}

std::string TestDialect::compile(oos::detail::drop &drop)
{
  return "";
}

std::string TestDialect::compile(oos::detail::create &create)
{
  return token(create.type) + " " + create.table;
}
