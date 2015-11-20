//
// Created by sascha on 18.09.15.
//
#include "database/statement_impl.hpp"

#include "object/serializable.hpp"

namespace oos {

namespace detail {

statement_impl::~statement_impl()
{}

int statement_impl::bind(serializable *o)
{
  reset();
  host_index = 0;
  o->serialize(*this);
  return host_index;
}

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