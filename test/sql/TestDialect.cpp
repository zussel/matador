//
// Created by sascha on 2/26/16.
//

#include "TestDialect.hpp"

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect_linker.hpp"

#include <algorithm>

using namespace matador;

TestDialect::TestDialect()
  : basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{ }

const char* TestDialect::type_string(data_type type) const
{
  switch(type) {
    case data_type::type_char:
      return "INTEGER";
    case data_type::type_short:
      return "INTEGER";
    case data_type::type_int:
      return "INTEGER";
    case data_type::type_long:
      return "INTEGER";
    case data_type::type_unsigned_char:
      return "INTEGER";
    case data_type::type_unsigned_short:
      return "INTEGER";
    case data_type::type_unsigned_int:
      return "INTEGER";
    case data_type::type_unsigned_long:
      return "INTEGER";
    case data_type::type_bool:
      return "INTEGER";
    case data_type::type_float:
      return "DOUBLE";
    case data_type::type_double:
      return "DOUBLE";
    case data_type::type_char_pointer:
      return "VARCHAR";
    case data_type::type_varchar:
      return "VARCHAR";
    case data_type::type_text:
      return "TEXT";
    case data_type::type_date:
      return "REAL";
    case data_type::type_time:
      return "TEXT";
    default:
    {
      std::stringstream msg;
      msg << "test dialect: unknown data type [" << (int)type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

dialect_traits::identifier TestDialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_BOTH_SAME;
}
