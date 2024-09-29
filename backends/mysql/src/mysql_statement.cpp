#include "mysql_statement.hpp"
#include "mysql_error.hpp"
#include "mysql_prepared_result_reader.hpp"

namespace matador::backends::mysql {

mysql_statement::mysql_statement(MYSQL_STMT *stmt, const sql::query_context &query)
: statement_impl(query)
, stmt_(stmt)
, binder_(query_.bind_vars.size())
{}

size_t mysql_statement::execute()
{
  if (!binder_.bind_params().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_.bind_params().data()) != 0) {
      throw_mysql_error(stmt_, "mysql", query_.sql);
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    throw_mysql_error(stmt_, "mysql", query_.sql);
  }

  return mysql_stmt_affected_rows(stmt_);
}

std::unique_ptr<sql::query_result_impl> mysql_statement::fetch()
{
  if (!binder_.bind_params().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_.bind_params().data()) != 0) {
      throw_mysql_error(stmt_, "mysql", query_.sql);
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    throw_mysql_error(stmt_, "mysql", query_.sql);
  }
  if (mysql_stmt_store_result(stmt_) != 0) {
    throw_mysql_error(stmt_, "mysql", query_.sql);
  }

  return std::move(std::make_unique<sql::query_result_impl>(std::make_unique<mysql_prepared_result_reader>(stmt_), std::move(query_.prototype)));
}

void mysql_statement::reset() {}

object::attribute_writer& mysql_statement::binder()
{
  return binder_;
}

}