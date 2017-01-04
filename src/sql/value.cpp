//
// Created by sascha on 3/2/16.
//
#include "oos/sql/value.hpp"
#include "oos/sql/token_visitor.hpp"
#include "oos/sql/basic_dialect.hpp"

namespace oos {
namespace detail {

void basic_value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

}

std::string null_value::NULLSTR = "NULL";

void null_value::serialize(const char *id, serializer &srlzr)
{
  srlzr.serialize(id, NULLSTR);
}

std::string null_value::str() const
{
  return NULLSTR;
}

std::string null_value::safe_string(const basic_dialect &) const
{
  return NULLSTR;
}

const char *null_value::type_id() const {
  return "null";
}

detail::basic_value* make_value(const char* val, size_t len)
{
  return new value<char*>(val, len);
}

}