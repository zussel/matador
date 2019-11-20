//
// Created by sascha on 3/3/16.
//
#include "matador/db/sqlite/sqlite_dialect.hpp"
#include "matador/db/sqlite/sqlite_dialect_compiler.hpp"

#include "matador/sql/basic_dialect_linker.hpp"

#include <algorithm>

namespace matador {

namespace sqlite {


sqlite_dialect::sqlite_dialect()
  : basic_dialect(new sqlite_dialect_compiler, new detail::basic_dialect_linker)
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT TRANSACTION");
  replace_token(detail::token::ROLLBACK, "ROLLBACK TRANSACTION");
}

const char *sqlite_dialect::type_string(matador::data_type type) const
{
  switch(type) {
    case data_type::type_char:
      return "TINYINT";
    case data_type::type_short:
      return "SMALLINT";
    case data_type::type_int:
      return "INTEGER";
    case data_type::type_long:
      return "BIGINT";
    case data_type::type_long_long:
      return "BIGINT";
    case data_type::type_unsigned_char:
      return "TINYINT UNSIGNED";
    case data_type::type_unsigned_short:
      return "SMALLINT UNSIGNED";
    case data_type::type_unsigned_int:
      return "INTEGER UNSIGNED";
    case data_type::type_unsigned_long:
      return "BIGINT UNSIGNED";
    case data_type::type_unsigned_long_long:
      return "BIGINT UNSIGNED";
    case data_type::type_bool:
      return "BOOLEAN";
    case data_type::type_float:
      return "FLOAT";
    case data_type::type_double:
      return "DOUBLE";
    case data_type::type_char_pointer:
      return "VARCHAR";
    case data_type::type_varchar:
      return "VARCHAR";
    case data_type::type_text:
      return "TEXT";
    case data_type::type_date:
      return "DATE";
    case data_type::type_time:
      return "DATETIME";
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
  if (strncmp(type, "INTEGER", 7) == 0) {
    if (strstr(type, "UNSIGNED") != nullptr) {
      return data_type::type_unsigned_int;
    } else {
      return data_type::type_int;
    }
    return data_type::type_int;
  } else if (strncmp(type, "TINYINT", 3) == 0) {
    if (strstr(type, "UNSIGNED") != nullptr) {
      return data_type::type_unsigned_char;
    } else {
      return data_type::type_char;
    }
  } else if (strncmp(type, "SMALLINT", 8) == 0) {
    if (strstr(type, "UNSIGNED") != nullptr) {
      return data_type::type_unsigned_short;
    } else {
      return data_type::type_short;
    }
  } else if (strncmp(type, "BIGINT", 6) == 0) {
    if (strstr(type, "UNSIGNED") != nullptr) {
      return data_type::type_unsigned_long_long;
    } else {
      return data_type::type_long_long;
    }
  } else if (strcmp(type, "REAL") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "FLOAT") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "DOUBLE") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "DATE") == 0) {
    return data_type::type_date;
  } else if (strcmp(type, "DATETIME") == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
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