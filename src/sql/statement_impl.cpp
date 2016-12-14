//
// Created by sascha on 18.09.15.
//
#include "oos/sql/statement_impl.hpp"

namespace oos {

namespace detail {

statement_impl::~statement_impl()
{}

std::string statement_impl::str() const
{
  return sql_;
}

void statement_impl::str(const std::string &s)
{
  sql_ = s;
}

}

}