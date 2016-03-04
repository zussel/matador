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

}