//
// Created by sascha on 3/8/16.
//
#include "mysql_dialect.hpp"

#include "sql/dialect_token.hpp"
#include "sql/basic_dialect_compiler.hpp"
#include "sql/basic_dialect_linker.hpp"

#ifdef WIN32
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <algorithm>

namespace oos {

namespace mysql {

mysql_dialect::mysql_dialect()
  : basic_dialect(new detail::basic_dialect_compiler, new detail::basic_dialect_linker)
{
  replace_token(detail::token::BEGIN, "START TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT");
  replace_token(detail::token::ROLLBACK, "ROLLBACK");
}

const char* mysql_dialect::type_string(oos::data_type type) const
{
  switch(type) {
    case data_type::type_char:
      return "CHAR(1)";
    case data_type::type_short:
      return "INTEGER";
    case data_type::type_int:
      return "INTEGER";
    case data_type::type_long:
	  return "INTEGER";
	  //return "BIGINT";
    case data_type::type_unsigned_char:
      return "CHAR(1)";
    case data_type::type_unsigned_short:
      return "INTEGER";
    case data_type::type_unsigned_int:
      return "INTEGER";
    case data_type::type_unsigned_long:
	  return "INTEGER";
	  //return "BIGINT";
	case data_type::type_bool:
      return "INTEGER";
    case data_type::type_float:
      return "FLOAT";
    case data_type::type_double:
      return "DOUBLE";
    case data_type::type_date:
      return "DATE";
    case data_type::type_time:
#if MYSQL_VERSION_ID < 50604
      // before mysql version 5.6.4 datetime
      // doesn't support fractional seconds
      // so we use a datetime string here
      return "VARCHAR(256)";
#else
      return "DATETIME(3)";
#endif
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

data_type mysql_dialect::string_type(const char *type) const
{
  if (strncmp(type, "int", 3) == 0) {
    return data_type::type_long;
  } else if (strcmp(type, "date") == 0) {
    return data_type::type_text;
  } else if (strncmp(type, "varchar", 7) == 0) {
    return data_type::type_varchar;
  } else {
    return data_type::type_unknown;
  }
}

}

}
