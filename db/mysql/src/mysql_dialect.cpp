//
// Created by sascha on 3/8/16.
//
#include "mysql_dialect.hpp"

#include "sql/dialect_token.hpp"

#include <algorithm>
#include <cstring>

namespace oos {

namespace mysql {


mysql_dialect::mysql_dialect()
{
  replace_token(detail::token::BEGIN, "START TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT");
  replace_token(detail::token::ROLLBACK, "ROLLBACK");
}

const char* mysql_dialect::type_string(oos::data_type_t type) const
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
      return "FLOAT";
    case type_double:
      return "DOUBLE";
    case type_date:
      return "DATE";
    case type_time:
#if MYSQL_VERSION_ID < 50604
      // before mysql version 5.6.4 datetime
      // doesn't support fractional seconds
      // so we use a datetime string here
      return "VARCHAR(256)";
#else
      return "DATETIME(3)";
#endif
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
      return "TEXT";
    default:
    {
      std::stringstream msg;
      msg << "mysql sql: unknown type xxx [" << type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

data_type_t mysql_dialect::string_type(const char *type) const
{
  if (strncmp(type, "int", 3) == 0) {
    return type_long;
  } else if (strcmp(type, "date") == 0) {
    return type_text;
  } else if (strncmp(type, "varchar", 7) == 0) {
    return type_varchar;
  } else {
    return type_unknown;
  }
}

void mysql_dialect::reset()
{
  basic_dialect::reset();
  bind_count_ = 0; column_count_ = 0;
}

void mysql_dialect::visit(const oos::detail::basic_value &value)
{
  if (compile_type() == DIRECT) {
    append_to_result(value.str());
  } else {
    ++bind_count_;
    append_to_result("?");
  }
}

void mysql_dialect::visit(const oos::column &col)
{
  ++column_count_;
  basic_dialect::visit(col);
}

size_t mysql_dialect::bind_count() const
{
  return bind_count_;
}

size_t mysql_dialect::column_count() const
{
  return column_count_;
}

}

}
