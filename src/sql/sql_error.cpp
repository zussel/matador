#include "matador/sql/sql_error.hpp"

namespace matador::sql {

sql_error::sql_error(const sql_error_code ec,
                     std::string native_error,
                     std::string error_message,
                     std::string dbms,
                     std::string sql_statement)
: error_code_(ec)
, native_error_(std::move(native_error))
, error_message_(std::move(error_message))
, dbms_(std::move(dbms))
, sql_statement_(std::move(sql_statement))
{}

sql_error_code sql_error::ec() const
{
  return error_code_;
}

std::string sql_error::native_error() const
{
  return native_error_;
}

std::string sql_error::error_message() const
{
  return error_message_;
}

std::string sql_error::dbms() const
{
  return dbms_;
}

std::string sql_error::sql_statement() const
{
  return sql_statement_;
}
}