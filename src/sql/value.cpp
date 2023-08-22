#include "matador/sql/value.hpp"
#include "matador/sql/token_visitor.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

void value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
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