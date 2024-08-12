#include "odbc_statement.hpp"
#include "odbc_prepared_result_reader.hpp"
#include "odbc_error.hpp"

namespace matador::backends::odbc {
odbc_statement::odbc_statement(sqlite3 *db, sqlite3_stmt *stmt, const sql::query_context &query)
: statement_impl(query)
, db_(db)
, stmt_(stmt)
, binder_(db, stmt)
{}

odbc_statement::~odbc_statement()
{
  sqlite3_finalize(stmt_);
}

size_t odbc_statement::execute()
{
  // get next row
  int ret = sqlite3_reset(stmt_);
  throw_odbc_error(ret, db_, "sqlite3_reset");
  if (ret = sqlite3_step(stmt_); ret != odbc_DONE) {
    throw_odbc_error(ret, db_, "sqlite3_step");
  }

  return sqlite3_changes(db_);
}

std::unique_ptr<sql::query_result_impl> odbc_statement::fetch()
{
  int ret = sqlite3_reset(stmt_);
  throw_odbc_error(ret, db_, "sqlite3_reset");

  auto reader = std::make_unique<odbc_prepared_result_reader>(db_, stmt_);
  return std::move(std::make_unique<sql::query_result_impl>(std::move(reader), query_.prototype));
}

void odbc_statement::reset()
{
  if (stmt_) {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }
}

object::attribute_binder& odbc_statement::binder()
{
  return binder_;
}

}