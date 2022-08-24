#include "matador/db/sqlite/sqlite_statement.hpp"
#include "matador/db/sqlite/sqlite_connection.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"
#include "matador/db/sqlite/sqlite_prepared_result.hpp"

#include "matador/sql/row.hpp"

#include <sqlite3.h>

namespace matador {

namespace sqlite {

sqlite_statement::sqlite_statement(sqlite_connection &db, const matador::sql &sql)
  : statement_impl(db.dialect(), sql)
  , db_(db)
  , stmt_(nullptr)
{
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_.handle(), str().c_str(), str().size(), &stmt_, nullptr);
  throw_database_error(ret, db_.handle(), "sqlite3_prepare_v2", str());
  binder_ = std::make_unique<sqlite_parameter_binder>(db.handle(), stmt_);
}

sqlite_statement::~sqlite_statement()
{
  clear();
}

detail::result_impl* sqlite_statement::execute()
{
  // get next row
  sqlite3_reset(stmt_);
  int ret = sqlite3_step(stmt_);

  if (ret != SQLITE_ROW && ret != SQLITE_DONE) {
    throw_database_error(ret, db_.handle(), "sqlite3_step");
  }
  return new sqlite_prepared_result(stmt_, ret);
}

void sqlite_statement::reset()
{
  if (stmt_) {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }
}

void sqlite_statement::clear()
{
  if (!stmt_) {
    return;
  }
  int ret = sqlite3_finalize(stmt_);
  throw_database_error(ret, db_.handle(), "sqlite3_finalize");
  stmt_ = nullptr;
}

detail::parameter_binder_impl *sqlite_statement::binder() const
{
  return binder_.get();
}

}

}
