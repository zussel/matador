//
// Created by sascha on 3/3/16.
//
#include "sqlite_dialect.hpp"
#include "sqlite_dialect_compiler.hpp"

#include "sql/basic_dialect_linker.hpp"

#include <algorithm>

namespace oos {

namespace sqlite {


sqlite_dialect::sqlite_dialect()
  : basic_dialect(new sqlite_dialect_compiler(*this), new detail::basic_dialect_linker)
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT TRANSACTION");
  replace_token(detail::token::ROLLBACK, "ROLLBACK TRANSACTION");
}

const char *sqlite_dialect::type_string(oos::data_type type) const
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
      return "TEXT";
    case data_type::type_time:
      return "TEXT";
    default: {
      std::stringstream msg;
      msg << "sqlite sql: unknown type [" << (int)type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

data_type sqlite_dialect::string_type(const char *type) const
{
  if (strcmp(type, "INTEGER") == 0) {
    return data_type::type_long;
  } else if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
  } else if (strcmp(type, "REAL") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  } else {
    return data_type::type_unknown;
  }
}

dialect_traits::identifier sqlite_dialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_BOTH_SAME;
}

}

}