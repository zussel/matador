//
// Created by sascha on 3/4/16.
//
#include "sql/basic_dialect.hpp"
#include "sql/sql.hpp"
#include "sql/dialect_token.hpp"

namespace oos {

std::string basic_dialect::direct(const sql &s)
{
  compile_type_ = DIRECT;
  return s.compile(*this);
}

std::string basic_dialect::prepare(const sql &s)
{
  compile_type_ = PREPARED;
  return s.compile(*this);
}

bool basic_dialect::is_preparing() const
{
  return compile_type_ == PREPARED;
}

std::string basic_dialect::result() const
{
  return result_;
}

void basic_dialect::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " " << top.limit_ << " ";
  append_to_result(res.str());
}

void basic_dialect::visit(const oos::detail::remove &)
{

}

void basic_dialect::replace_token(detail::token::t_token tkn, const std::string &value)
{
  tokens[tkn] = value;
}

void basic_dialect::append_to_result(const std::string &part)
{
  result_ += part;
}

}