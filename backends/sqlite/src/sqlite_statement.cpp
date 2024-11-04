#include "sqlite_statement.hpp"
#include "sqlite_prepared_result_reader.hpp"
#include "sqlite_error.hpp"

#include "matador/sql/sql_error.hpp"

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

utils::result<size_t, sql::sql_error> sqlite_statement::execute()
{
  // get next row
  int ret = sqlite3_reset(stmt_);
  if (ret != SQLITE_OK && ret != SQLITE_DONE) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, std::to_string(ret), sqlite3_errmsg(db_), "sqlite3"});
  }

  if (ret = sqlite3_step(stmt_); ret != SQLITE_DONE) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, std::to_string(ret), sqlite3_errmsg(db_), "sqlite3"});
  }

  return utils::ok(static_cast<size_t>(sqlite3_changes(db_)));
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> sqlite_statement::fetch()
{
  const int ret = sqlite3_reset(stmt_);
  if (ret != SQLITE_OK && ret != SQLITE_DONE) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, std::to_string(ret), sqlite3_errmsg(db_), "sqlite3"});
  }

  auto reader = std::make_unique<sqlite_prepared_result_reader>(db_, stmt_);
  return utils::ok(std::make_unique<sql::query_result_impl>(std::move(reader), query_.prototype));
}

void sqlite_statement::reset()
{
  if (stmt_) {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }
}

utils::attribute_writer& sqlite_statement::binder()
{
  return binder_;
}

}