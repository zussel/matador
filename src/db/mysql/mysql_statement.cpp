#include "matador/db/mysql/mysql_statement.hpp"
#include "matador/db/mysql/mysql_connection.hpp"
#include "matador/db/mysql/mysql_exception.hpp"
#include "matador/db/mysql/mysql_prepared_result.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/sql/sql.hpp"

namespace matador {

namespace mysql {

mysql_statement::mysql_statement(mysql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , stmt_(mysql_stmt_init(db.handle()))
{
  if (mysql_stmt_prepare(stmt_, str().c_str(), static_cast<unsigned long>(str().size())) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }

  binder_ = std::make_unique<mysql_parameter_binder>(columns().size(), bind_vars().size());
}

mysql_statement::~mysql_statement()
{
  clear();
  mysql_stmt_close(stmt_);
}

void mysql_statement::reset()
{
  mysql_stmt_reset(stmt_);
}

void mysql_statement::unlink_result(mysql_prepared_result *result)
{
  if (result == current_result) {
    current_result = nullptr;
  }
}

void mysql_statement::clear()
{
  binder_->reset();
  if (current_result != nullptr) {
    current_result->free();
  } else {
    mysql_stmt_free_result(stmt_);
  }
}

detail::result_impl* mysql_statement::execute()
{
  if (current_result != nullptr) {
    current_result->free();
  }
  if (!binder_->host_array().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_->host_array().data()) != 0) {
      throw_stmt_error(stmt_, "mysql", str());
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }
  if (mysql_stmt_store_result(stmt_) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }
  current_result = new mysql_prepared_result(this, stmt_, binder_->bindings(), binder_->result_infos());
  return current_result;
}

detail::parameter_binder_impl *mysql_statement::binder() const
{
  return binder_.get();
}

}

}
