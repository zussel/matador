//
// Created by sascha on 3/2/16.
//
#include "matador/sql/value.hpp"
#include "matador/sql/token_visitor.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {
//namespace detail {

void basic_value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

void basic_value::serialize(const char *id, serializer &srlzr)
{
  value_visitor_.apply(value_, id, &srlzr);
}

std::string basic_value::str()
{
  return value_to_string_visitor_.to_string(value_)
}

std::string basic_value::safe_string(const basic_dialect &d)
{
  return value_to_string_visitor_.to_safe_string(value_, &d);
}

const char *basic_value::type_id() const
{
  return value_.type_index().name();
}
//}

std::string null_value::NULLSTR = "NULL";

const char *null_value::type_id() const {
  return "null";
}

basic_value* make_value(const char* val, size_t len)
{
  return new basic_value(val);
}

}