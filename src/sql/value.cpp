#include "matador/sql/value.hpp"
#include "matador/sql/token_visitor.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

void value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

void value::serialize(const char *id, serializer &srlzr)
{
  value_visitor_.apply(value_, id, srlzr);
}

std::string value::str() const
{
  return const_cast<detail::value_to_string_visitor&>(value_to_string_visitor_).to_string(const_cast<any&>(value_));
}

std::string value::safe_string(const basic_dialect &d) const
{
  return const_cast<detail::value_to_string_visitor&>(value_to_string_visitor_).to_safe_string(const_cast<any&>(value_), &d);
}

const char *value::type_id() const
{
  return value_.type_index().name();
}

std::string null_value::NULLSTR = "NULL";

const char *null_value::type_id() const {
  return "null";
}

value* make_value(const char* val, std::size_t len)
{
  return new value(val, len);
}

}