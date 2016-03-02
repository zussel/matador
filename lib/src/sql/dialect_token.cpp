//
// Created by sascha on 3/2/16.
//

#include "sql/dialect_token.hpp"

namespace oos {

namespace detail {

std::string select::compile(basic_dialect &d, token::t_compile_type compile_type) const
{
  return d.compile(*this);
}

std::string drop::compile(basic_dialect &d, token::t_compile_type compile_type) const
{
  return d.compile(*this);
}

create::create(const std::string &t)
  : token(basic_dialect::CREATE_TABLE), table(t)
{}

std::string create::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
//  std::string result = token::compile(d, compile_type);
//  result += " " + table + " ";
//  return result;
}

insert::insert(const std::string &t)
  : token(basic_dialect::INSERT), table(t)
{}

std::string insert::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
//  std::string result = token::compile(d, compile_type);
//  result += table + " ";
//  return result;
}

update::update(const std::string &t)
  : token(basic_dialect::UPDATE)
{}

std::string update::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
//  std::string result = token::compile(d, compile_type);
//  result += table + " ";
//  return result;
}

remove::remove(const std::string &t)
  : token(basic_dialect::DELETE)
{}

std::string remove::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
//  std::string result = token::compile(d, compile_type);
//  result += table + " ";
//  return result;
}

std::string distinct::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

std::string set::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

std::string asc::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

std::string desc::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

from::from(const std::string &t)
  : token(basic_dialect::FROM), table(t)
{}

std::string from::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

limit::limit(size_t lmt)
  : token(basic_dialect::LIMIT), limit_(lmt)
{}

std::string limit::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
//  std::stringstream str;
//  str << token::compile(d, compile_type);
//  str << " (" << limit_ << ")";
//  return str.str();
}

order_by::order_by(const std::string &col)
  : token(basic_dialect::ORDER_BY), column(col)
{}

std::string order_by::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

group_by::group_by(const std::string &col)
  : token(basic_dialect::GROUP_BY), column(col)
{}

std::string group_by::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

std::string where::compile(basic_dialect &d, t_compile_type compile_type) const
{
  return d.compile(*this);
}

}
}