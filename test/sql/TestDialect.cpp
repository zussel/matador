//
// Created by sascha on 2/26/16.
//

#include "TestDialect.hpp"

#include "sql/dialect_token.hpp"

#include <algorithm>

using namespace oos;

TestDialect::TestDialect()
  : basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{ }

const char* TestDialect::type_string(data_type_t type) const
{
  switch(type) {
    case type_char:
      return "INTEGER";
    case type_short:
      return "INTEGER";
    case type_int:
      return "INTEGER";
    case type_long:
      return "INTEGER";
    case type_unsigned_char:
      return "INTEGER";
    case type_unsigned_short:
      return "INTEGER";
    case type_unsigned_int:
      return "INTEGER";
    case type_unsigned_long:
      return "INTEGER";
    case type_bool:
      return "INTEGER";
    case type_float:
      return "DOUBLE";
    case type_double:
      return "DOUBLE";
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
      return "TEXT";
    case type_date:
      return "REAL";
    case type_time:
      return "TEXT";
    default:
    {
      std::stringstream msg;
      msg << "test dialect: unknown data type [" << type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}
