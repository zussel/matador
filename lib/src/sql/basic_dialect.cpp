//
// Created by sascha on 3/4/16.
//
#include "sql/basic_dialect.hpp"
#include "sql/sql.hpp"

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

void basic_dialect::replace_token(detail::token::t_token tkn, const std::string &value)
{
  tokens[tkn] = value;
}

}