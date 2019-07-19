//
// Created by sascha on 18.09.15.
//
#include "matador/sql/statement_impl.hpp"

namespace matador {

namespace detail {

std::string statement_impl::str() const
{
  return sql_;
}

void statement_impl::str(const std::string &s)
{
  sql_ = s;
}

void statement_impl::log(const std::string &stmt) const
{
  std::cout << "SQL: " << stmt << "\n";
}

}

}