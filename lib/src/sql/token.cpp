//
// Created by sascha on 2/26/16.
//

#include "sql/token.hpp"

namespace oos {
namespace detail {

token::token(basic_dialect::t_token tok)
  : type(tok)
{}

std::string token::compile(basic_dialect *d, token::t_compile_type)
{
  std::stringstream part;
  part << d->token(type) << " ";
  return part.str();
}

create::create(const std::string &t)
  : token(basic_dialect::CREATE_TABLE), table(t)
{}

std::string create::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += " " + table + " ";
  return result;
}

insert::insert(const std::string &t)
  : token(basic_dialect::INSERT), table(t)
{}

std::string insert::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += table + " ";
  return result;
}

update::update(const std::string &t)
  : token(basic_dialect::UPDATE)
{}

std::string update::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += table + " ";
  return result;
}

remove::remove(const std::string &t)
  : token(basic_dialect::DELETE)
{}

std::string remove::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += table + " ";
  return result;
}

from::from(const std::string &t)
  : token(basic_dialect::FROM), table(t)
{}

std::string from::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += table + " ";
  return result;
}

limit::limit(size_t lmt)
  : token(basic_dialect::LIMIT), limit_(lmt)
{}

std::string limit::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::stringstream str;
  str << token::compile(d, compile_type);
  str << " (" << limit_ << ")";
  return str.str();
}

order_by::order_by(const std::string &col)
  : token(basic_dialect::ORDER_BY), column(col)
{}

std::string order_by::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += column + " ";
  return result;
}

group_by::group_by(const std::string &col)
  : token(basic_dialect::ORDER_BY), column(col)
{}

std::string group_by::compile(basic_dialect *d, t_compile_type compile_type)
{
  std::string result = token::compile(d, compile_type);
  result += column + " ";
  return result;
}

column::column(const std::string &col, data_type_t t, std::size_t idx, bool host)
  : token(basic_dialect::COLUMN)
  , name(col), type(t), index(idx), is_host(host)
{}

std::string column::compile(basic_dialect *d, t_compile_type compile_type)
{
  return name;
}

}
}