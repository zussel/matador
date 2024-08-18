#include "odbc_statement.hpp"
#include "odbc_prepared_result_reader.hpp"
#include "odbc_error.hpp"

namespace matador::backends::odbc {
odbc_statement::odbc_statement(SQLHANDLE stmt, const sql::query_context &query)
: statement_impl(query)
, stmt_(stmt)
, binder_(stmt)
{

}

odbc_statement::~odbc_statement()
{
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

size_t odbc_statement::execute()
{
  auto ret = SQLExecute(stmt_);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "odbc", query_.sql);

  // check if data is needed
  if (ret == SQL_NEED_DATA) {
    // put needed data from host_data

    // get first data to put
    PTR pid{ nullptr };
    ret = SQLParamData(stmt_, &pid);
    // get data from map
    auto data = binder_.get_data_to_put(pid);
    if (!data.has_value()) {
      throw std::logic_error("odbc couldn't find data to put");
    }
    // Todo
    // put data as long if it is requested
    while (ret == SQL_NEED_DATA) {
      auto &val = data->value().get();
      while (data->value().get().len > 256) {
        ret = SQLPutData(stmt_, val.data.get(), 256);
        val.len -= 256;
        val.data += 256;
      }
      ret = SQLPutData(stmt_, val->data, val->len);
      ret = SQLParamData(stmt_, &pid);
      if (!is_success(ret) && ret != SQL_NEED_DATA) {
        // error
        throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", str());
      } else if (ret == SQL_NEED_DATA) {
        // determine next column data pointer
        it = binder_->data_to_put_map().find(pid);
        if (it == binder_->data_to_put_map().end()) {
          throw std::logic_error("mssql couldn't find data to put");
        }
        val = it->second;
      }
    }
  } else {
    // check result
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", str());
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