//
// Created by sascha on 3/3/16.
//
#include "sqlite_dialect.hpp"

#include "sql/dialect_token.hpp"
#include "sql/condition.hpp"
#include "sql/column.hpp"

#include <sstream>
#include <algorithm>
#include <cstring>

namespace oos {

namespace sqlite {


sqlite_dialect::sqlite_dialect()
{
  replace_token(detail::token::BEGIN, "BEGIN TRANSACTION");
  replace_token(detail::token::COMMIT, "COMMIT TRANSACTION");
  replace_token(detail::token::ROLLBACK, "ROLLBACK TRANSACTION");
  replace_token(detail::token::TOP, "TOP");
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

std::string sqlite_dialect::compile(const oos::detail::top &top)
{
  std::stringstream res;
  res << token(top.type) << " " << top.limit_ << " ";
  return res.str();
}

std::string sqlite_dialect::compile(const oos::detail::as &alias)
{
  std::stringstream res;
  res << token(alias.type) << " " << alias.alias << " ";
  return res.str();
}

std::string sqlite_dialect::compile(const oos::detail::remove &remove1)
{
  return token(remove1.type) + " " + remove1.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::values &values)
{
  std::string result(token(values.type) + " (");

  if (values.values_.size() > 1) {
    std::for_each(values.values_.begin(), values.values_.end() - 1, [&](const std::shared_ptr<detail::basic_value> &val)
    {
      result += val->compile(*this);
      result += ", ";
    });
  }
  if (!values.values_.empty()) {
    result += values.values_.back()->compile(*this);
  }
  result += ") ";
  return result;
}

std::string sqlite_dialect::compile(const oos::detail::basic_value &value)
{
  if (compile_type() == DIRECT) {
    return value.str();
  } else {
    return "?";
  }
}

std::string sqlite_dialect::compile(const oos::detail::insert &insert)
{
  return token(insert.type) + " " + insert.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::group_by &by)
{
  return token(by.type) + " " + by.column + " ";
}

std::string sqlite_dialect::compile(const oos::detail::desc &desc)
{
  return token(desc.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::asc &asc)
{
  return token(asc.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::order_by &by)
{
  return token(by.type) + " " + by.column + " ";
}

std::string sqlite_dialect::compile(const oos::detail::basic_condition &cond)
{
  return cond.compile(*this);
}

std::string sqlite_dialect::compile(const oos::detail::where &where)
{
  return token(where.type) + " " + where.cond->compile(*this) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::from &from)
{
  return token(from.type) + " " + from.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::typed_varchar_column &column)
{
  std::stringstream str;
  str << column.name << " " << type_string(column.type) << "(" << column.size << ")";
  return str.str();
}

std::string sqlite_dialect::compile(const oos::detail::identifier_column &column)
{
  return column.name + " " + type_string(column.type) + " NOT NULL PRIMARY KEY";
}

std::string sqlite_dialect::compile(const oos::column &column)
{
  return column.name;
}

std::string sqlite_dialect::compile(const oos::detail::typed_column &column)
{
  return column.name + " " + type_string(column.type);
}

std::string sqlite_dialect::compile(const oos::columns &columns)
{
  std::string result;
  if (columns.with_brackets_ == columns::WITH_BRACKETS) {
    result += "(";
  }

  if (columns.columns_.size() > 1) {
    std::for_each(columns.columns_.begin(), columns.columns_.end() - 1, [&](const std::shared_ptr<column> &col)
    {
      result += col->compile(*this);
      result += ", ";
    });
  }
  if (!columns.columns_.empty()) {
    result += columns.columns_.back()->compile(*this);
  }

  if (columns.with_brackets_ == columns::WITH_BRACKETS) {
    result += ")";
  }

  result += " ";
  return result;
}

std::string sqlite_dialect::compile(const oos::detail::set &set)
{
  return token(set.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::update &update)
{
  return token(update.type) + " " + update.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::distinct &distinct)
{
  return token(distinct.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::select &select1)
{
  return token(select1.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::drop &drop)
{
  return token(drop.type) + " " + drop.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::create &create)
{
  return token(create.type) + " " + create.table + " ";
}

std::string sqlite_dialect::compile(const oos::detail::begin &bgn)
{
  return token(bgn.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::commit &cmmt)
{
  return token(cmmt.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::rollback &rllbck)
{
  return token(rllbck.type) + " ";
}

std::string sqlite_dialect::compile(const oos::detail::sql_token &s)
{
  return s.compile(*this);
}

}

}