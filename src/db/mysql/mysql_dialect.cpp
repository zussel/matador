//
// Created by sascha on 3/8/16.
//
#include "matador/db/mysql/mysql_dialect.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/basic_dialect_compiler.hpp"
#include "matador/sql/basic_dialect_linker.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <algorithm>

namespace matador {

namespace mysql {

mysql_dialect::mysql_dialect()
  : basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{
  replace_token(detail::token::BEGIN, "START TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT");
  replace_token(detail::token::ROLLBACK, "ROLLBACK");
  replace_token(detail::token::START_QUOTE, "`");
  replace_token(detail::token::END_QUOTE, "`");
}

const char* mysql_dialect::to_database_type_string(matador::data_type type) const
{
  switch(type) {
    case data_type::type_char:
      return "CHAR(1)";
    case data_type::type_short:
      return "SMALLINT";
    case data_type::type_int:
      return "INTEGER";
    case data_type::type_long:
	    return "BIGINT";
    case data_type::type_long_long:
	    return "BIGINT";
    case data_type::type_unsigned_char:
      return "CHAR(1)";
    case data_type::type_unsigned_short:
      return "SMALLINT UNSIGNED";
    case data_type::type_unsigned_int:
      return "INTEGER UNSIGNED";
    case data_type::type_unsigned_long:
	    return "BIGINT UNSIGNED";
    case data_type::type_unsigned_long_long:
	    return "BIGINT UNSIGNED";
	  case data_type::type_bool:
      return "INTEGER";
    case data_type::type_float:
      return "FLOAT";
    case data_type::type_double:
      return "DOUBLE";
    case data_type::type_date:
      return "DATE";
    case data_type::type_time:
      if (mysql::version < 50604) {
        // before mysql version 5.6.4 datetime
        // doesn't support fractional seconds
        // so we use a datetime string here
        return "VARCHAR(256)";
      } else {
        return "DATETIME(3)";
      }
    case data_type::type_char_pointer:
      return "VARCHAR";
    case data_type::type_varchar:
      return "VARCHAR";
    case data_type::type_text:
      return "TEXT";
    default:
    {
      std::stringstream msg;
      msg << "mysql sql: unknown type xxx [" << (int)type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

database_type mysql_dialect::string_type(const char *type) const
{
  if (strncmp(type, "int", 3) == 0) {
    if (strstr(type, "unsigned") != nullptr) {
      return database_type::type_bigint;
    } else {
      return database_type::type_bigint;
    }
  } else if (strncmp(type, "bigint", 6) == 0) {
    if (strstr(type, "unsigned") != nullptr) {
      return database_type::type_bigint;
    } else {
      return database_type::type_bigint;
    }
  } else if (strcmp(type, "date") == 0) {
    return database_type::type_date;
  } else if (strncmp(type, "datetime", 8) == 0) {
    return database_type::type_time;
  } else if (strcmp(type, "float") == 0) {
    return database_type::type_float;
  } else if (strcmp(type, "double") == 0) {
    return database_type::type_double;
  } else if (strncmp(type, "varchar", 7) == 0) {
    return database_type::type_varchar;
  } else if (strncmp(type, "text", 0) == 0) {
    return database_type::type_text;
  } else {
    return database_type::type_unknown;
  }
}

dialect_traits::identifier mysql_dialect::identifier_escape_type() const
{
  return dialect_traits::ESCAPE_BOTH_SAME;
}

}

}
