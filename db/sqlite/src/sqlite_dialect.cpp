//
// Created by sascha on 3/3/16.
//
#include "sqlite_dialect.hpp"
#include "sqlite_dialect_compiler.hpp"

#include "sql/dialect_token.hpp"
#include "sql/condition.hpp"
#include "sql/column.hpp"

#include <sstream>
#include <algorithm>
#include <cstring>

namespace oos {

namespace sqlite {


sqlite_dialect::sqlite_dialect()
  : basic_dialect(new sqlite_dialect_compiler)
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT TRANSACTION");
  replace_token(detail::token::ROLLBACK, "ROLLBACK TRANSACTION");
}

const char *sqlite_dialect::type_string(oos::data_type_t type) const
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
      return "TEXT";
    case type_time:
      return "TEXT";
    default: {
      std::stringstream msg;
      msg << "sqlite sql: unknown type xxx [" << type << "]";
      throw std::logic_error(msg.str());
      //throw std::logic_error("mysql sql: unknown type");
    }
  }
}

data_type_t sqlite_dialect::string_type(const char *type) const
{
  if (strcmp(type, "INTEGER") == 0) {
    return type_long;
  } else if (strcmp(type, "TEXT") == 0) {
    return type_text;
  } else if (strcmp(type, "REAL") == 0) {
    return type_double;
  } else if (strcmp(type, "BLOB") == 0) {
    return type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return type_varchar;
  } else {
    return type_unknown;
  }
}

void sqlite_dialect::parse(token_list_t &tokens) const
{
  if (tokens.empty()) {
    return;
  }

  token_ptr &first = tokens.front();

  if (first->type == detail::token::UPDATE || first->type == detail::token::DELETE) {
    // check if limit exists
    auto limit = std::find_if(tokens.begin(), tokens.end(), [](const token_ptr &item) {
      return item->type == detail::token::TOP;
    });

    if (limit == tokens.end()) {
      // no limit
      return;
    }

    // replace limit with sub select

  }
}

}

}