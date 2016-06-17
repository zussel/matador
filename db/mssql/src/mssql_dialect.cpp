//
// Created by sascha on 3/9/16.
//

#include "mssql_dialect.hpp"
#include "mssql_dialect_compiler.hpp"

#include "sql/dialect_token.hpp"

#include <algorithm>
#include <cstring>

namespace oos {

namespace mssql {

mssql_dialect::mssql_dialect()
  : basic_dialect(new mssql_dialect_compiler)
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT");
  replace_token(detail::token::ROLLBACK, "ROLLBACK");
  replace_token(detail::token::TOP, "TOP");
}

const char* mssql_dialect::type_string(oos::data_type_t type) const
{
  switch(type) {
    case type_char:
      return "CHAR(1)";
    case type_short:
      return "SMALLINT";
    case type_int:
      return "INT";
    case type_long:
      return "BIGINT";
    case type_unsigned_char:
      return "SMALLINT";
    case type_unsigned_short:
      return "INT";
    case type_unsigned_int:
      return "BIGINT";
    case type_unsigned_long:
      return "NUMERIC(21,0)";
    case type_bool:
      return "BIT";
    case type_float:
      return "FLOAT";
    case type_double:
      return "FLOAT";
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
      return "TEXT";
    case type_date:
      return "DATE";
    case type_time:
      return "DATETIME";
    default:
    {
      std::stringstream msg;
      msg << "mssql connection: unknown type xxx [" << type << "]";
      throw std::logic_error(msg.str());
    }
  }
}

data_type_t mssql_dialect::string_type(const char *type) const
{
  if (strcmp(type, "numeric") == 0) {
    return type_long;
  } else if (strcmp(type, "bigint") == 0) {
    return type_long;
  } else if (strcmp(type, "date") == 0) {
    return type_text;
  } else if (strcmp(type, "varchar") == 0) {
    return type_varchar;
  } else {
    return type_unknown;
  }
}

void mssql_dialect::visit(const oos::detail::top &top)
{
  std::stringstream res;
  res << token_string(top.type) << " (" << top.limit_ << ") ";
  append_to_result(res.str());
}

}

}
