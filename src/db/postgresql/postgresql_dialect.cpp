//
// Created by sascha on 24.05.19.
//

#include "matador/db/postgresql/postgresql_dialect.hpp"

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect_linker.hpp"

namespace matador {
namespace postgresql {

postgresql_dialect::postgresql_dialect()
: basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{

}

const char *postgresql_dialect::type_string(matador::data_type type) const
{
  switch (type) {
    case data_type::type_char:
      return "CHAR(1)";
    case data_type::type_short:
      return "SMALLINT";
    case data_type::type_int:
      return "INTEGER";
    case data_type::type_long:
      //return "INTEGER";
      return "BIGINT";
    case data_type::type_unsigned_char:
      return "CHAR(1)";
    case data_type::type_unsigned_short:
      return "INTEGER";
    case data_type::type_unsigned_int:
      return "BIGINT";
    case data_type::type_unsigned_long:
      //return "INTEGER";
      return "BIGINT";
    case data_type::type_bool:
      return "INTEGER";
    case data_type::type_float:
      return "FLOAT(4)";
    case data_type::type_double:
      return "REAL";
    case data_type::type_date:
      return "DATE";
    case data_type::type_time:
      return "TIMESTAMP";
    case data_type::type_char_pointer:
      return "VARCHAR";
    case data_type::type_varchar:
      return "VARCHAR";
    case data_type::type_text:
      return "TEXT";
    default:
    {
      std::stringstream msg;
      msg << "postgresql sql: unknown type xxx [" << (int)type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

matador::data_type postgresql_dialect::string_type(const char *type) const
{
  if (strncmp(type, "int", 3) == 0) {
    return data_type::type_int;
  } else if (strncmp(type, "bigint", 6) == 0) {
    return data_type::type_long;
  } else if (strcmp(type, "date") == 0) {
    return data_type::type_date;
  } else if (strncmp(type, "timestamp", 8) == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "float") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "double") == 0) {
    return data_type::type_double;
  } else if (strncmp(type, "varchar", 7) == 0) {
    return data_type::type_varchar;
  } else if (strncmp(type, "text", 0) == 0) {
    return data_type::type_text;
  } else {
    return data_type::type_unknown;
  }
}

matador::dialect_traits::identifier postgresql_dialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_BOTH_SAME;
}

}
}
