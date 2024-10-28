#include "mysql_statement.hpp"
#include "mysql_error.hpp"
#include "mysql_prepared_result_reader.hpp"
#include "mysql_result_binder.hpp"

#include "matador/sql/sql_error.hpp"

namespace matador::backends::mysql {

mysql_statement::mysql_statement(MYSQL_STMT *stmt, const sql::query_context &query)
: statement_impl(query)
, stmt_(stmt)
, binder_(query_.bind_vars.size())
{}

mysql_statement::~mysql_statement()
{
  mysql_stmt_close(stmt_);
}

utils::result<size_t, sql::sql_error> mysql_statement::execute()
{
  if (!binder_.bind_params().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_.bind_params().data()) != 0) {
      return utils::error(make_error(sql::sql_error_code::BIND_FAILED, stmt_, query_.sql));
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, stmt_, query_.sql));
  }

  return utils::ok(static_cast<size_t>(mysql_stmt_affected_rows(stmt_)));
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> mysql_statement::fetch()
{
  if (!binder_.bind_params().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_.bind_params().data()) != 0) {
      return utils::error(make_error(sql::sql_error_code::BIND_FAILED, stmt_, query_.sql));
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, stmt_, query_.sql));
  }
  if (mysql_stmt_store_result(stmt_) != 0) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, stmt_, query_.sql));
  }


  auto *prepare_meta_result = mysql_stmt_result_metadata(stmt_);
  if (prepare_meta_result == nullptr) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, "Failed to retrieve result meta data", query_.sql));
  }

  mysql_result_binder result_binder(query_.result_vars.size());

  return utils::ok(std::make_unique<sql::query_result_impl>(
    std::make_unique<mysql_prepared_result_reader>(stmt_, prepare_meta_result, std::move(result_binder)),
    std::move(query_.prototype))
  );
}

void mysql_statement::reset()
{
  if (mysql_stmt_reset(stmt_) > 0) {
    // Todo: handle mysql stmt reset error
  }
}

object::attribute_writer& mysql_statement::binder()
{
  return binder_;
}

}