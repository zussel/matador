#include "sqlite_statement.hpp"
#include "sqlite_prepared_result_reader.hpp"
#include "sqlite_error.hpp"

namespace matador::backends::sqlite {
sqlite_statement::sqlite_statement(sqlite3 *db, sqlite3_stmt *stmt, const sql::query_context &query)
: statement_impl(query)
, db_(db)
, stmt_(stmt)
, binder_(db, stmt)
{}

sqlite_statement::~sqlite_statement()
{
  sqlite3_finalize(stmt_);
}

size_t sqlite_statement::execute()
{
  // get next row
  int ret = sqlite3_reset(stmt_);
  throw_sqlite_error(ret, db_, "sqlite3_reset");
  if (ret = sqlite3_step(stmt_); ret != SQLITE_DONE) {
    throw_sqlite_error(ret, db_, "sqlite3_step");
  }

  return sqlite3_changes(db_);
}

std::unique_ptr<sql::query_result_impl> sqlite_statement::fetch()
{
  const int ret = sqlite3_reset(stmt_);
  throw_sqlite_error(ret, db_, "sqlite3_reset");

  auto reader = std::make_unique<sqlite_prepared_result_reader>(db_, stmt_);
  return std::make_unique<sql::query_result_impl>(std::move(reader), query_.prototype);
}

void sqlite_statement::reset()
{
  if (stmt_) {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }
}

object::attribute_binder& sqlite_statement::binder()
{
  return binder_;
}

}