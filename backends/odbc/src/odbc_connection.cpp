#include "odbc_connection.hpp"
#include "odbc_error.hpp"
#include "odbc_result_reader.hpp"
#include "odbc_statement.hpp"

#include "matador/sql/record.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/string.hpp"

#include <cstring>
#include <memory>
#include <utility>

#include <sql.h>
#include <sqlext.h>

namespace matador::backends::odbc {
odbc_connection::odbc_connection(const sql::connection_info &info)
  : connection_impl(info) {
}

void odbc_connection::open() {
  if (is_open()) {
    return;
  }

  auto res = odbc_.allocate().and_then([this]() {
    return odbc_.set_attribute(SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
  }).and_then([this]() {
    return connection_.allocate(odbc_);
  }).and_then([this]() {
    return connection_.set_attribute(SQL_LOGIN_TIMEOUT, (SQLPOINTER *) 5, 0);
  }).or_else([this](auto err) {
    std::ignore = connection_.free();
    std::ignore = odbc_.free();
    return utils::error(err);
  });

  const std::string dns(
    "DRIVER={" + info().driver + "};SERVER=" + info().hostname + ";Protocol=TCPIP;Port=" + std::to_string(info().port) +
    ";DATABASE=" + info().database + ";UID=" + info().user + ";PWD=" + info().password + ";");

  SQLCHAR retconstring[1024];
  auto ret = SQLDriverConnect(connection_.handle(), nullptr, (SQLCHAR *) dns.c_str(), SQL_NTS, retconstring, 1024, nullptr,
                         SQL_DRIVER_NOPROMPT);

  if (ret != SQL_SUCCESS) {
    make_error(sql::sql_error_code::OPEN_ERROR, ret, SQL_HANDLE_DBC, connection_.handle());
  }
}

void odbc_connection::close() {
  if (!is_open()) {
    return;
  }

  if (SQLRETURN ret = SQLDisconnect(connection_.handle()); ret != SQL_SUCCESS) {
    make_error(sql::sql_error_code::OPEN_ERROR, ret, SQL_HANDLE_DBC, connection_.handle());
  }

  auto res = connection_.free();
  if (!res.is_ok()) {
    return;
  }
  res = odbc_.free();
  if (!res.is_ok()) {
    return;
  }
}

bool odbc_connection::is_open() const {
  return connection_.is_valid();
}

bool odbc_connection::is_valid() const {
  SQLUINTEGER connectionDead;
  const SQLRETURN ret = SQLGetConnectAttr(connection_.handle(), SQL_ATTR_CONNECTION_DEAD, &connectionDead, 0, nullptr);
  return (ret == SQL_SUCCESS && connectionDead == SQL_CD_FALSE);
}


utils::result<size_t, sql::sql_error> odbc_connection::execute(const std::string &sql) {
  if (!connection_.is_valid()) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, "", "mssql no odbc connection established", "odbc"});
  }
  // create statement handle
  const auto stmt = execute_statement(sql);
  if (!stmt.is_ok()) {
    return utils::error(stmt.err());
  }

  SQLLEN affected_rows{0};
  if (const auto ret = SQLRowCount(*stmt, &affected_rows); !is_succeeded_or_no_data(ret)) {
    return utils::error(make_error( sql::sql_error_code::EXECUTE_FAILED, ret, SQL_HANDLE_STMT, *stmt, sql ));
  }

  if (affected_rows < 0) {
    affected_rows = 0;
  }
  return utils::ok(static_cast<size_t>(affected_rows));
}

utils::result<std::unique_ptr<sql::query_result_impl>, sql::sql_error> odbc_connection::fetch(const sql::query_context &context) {
  if (!connection_.is_valid()) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, "", "mssql no odbc connection established", "odbc"});
  }
  // create statement handle
  const auto stmt = execute_statement(context.sql);
  if (!stmt.is_ok()) {
    return utils::error(stmt.err());
  }

  SQLSMALLINT num_columns{};
  if (const auto ret = SQLNumResultCols(*stmt, &num_columns); !is_succeeded_or_no_data(ret)) {
    utils::error(make_error(sql::sql_error_code::FETCH_FAILED, ret, SQL_HANDLE_STMT, *stmt, context.sql));
  }

  std::vector<sql::column_definition> columns;
  for (SQLSMALLINT i = 1; i <= num_columns; i++) {
    auto column = describe_column(*stmt, i);
    if (!column.is_ok()) {
      return utils::error(column.err());
    }
    columns.push_back(*column);
  }

  return utils::ok(std::make_unique<sql::query_result_impl>(std::make_unique<odbc_result_reader>(*stmt), std::move(columns), 1));
}

utils::result<std::unique_ptr<sql::statement_impl>, sql::sql_error> odbc_connection::prepare(sql::query_context query) {
  // create statement handle
  SQLHANDLE stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_.handle(), &stmt);
  if (!is_succeeded_or_no_data(ret)) {
    utils::error(make_error(sql::sql_error_code::PREPARE_FAILED, ret, SQL_HANDLE_DBC, connection_.handle(), query.sql));
  }

  ret = SQLPrepare(stmt, (SQLCHAR *) query.sql.c_str(), SQL_NTS);
  if (!is_succeeded_or_no_data(ret)) {
    utils::error(make_error(sql::sql_error_code::PREPARE_FAILED, ret, SQL_HANDLE_STMT, stmt, query.sql));
  }

  std::unique_ptr<sql::statement_impl> impl(std::make_unique<odbc_statement>(stmt, query));
  return utils::ok(std::move(impl));
}

data_type string2type(const char *type) {
  if (strncmp(type, "INTEGER", 7) == 0) {
    return data_type::type_int;
  } else if (strncmp(type, "TINYINT", 7) == 0) {
    return data_type::type_char;
  } else if (strncmp(type, "SMALLINT", 8) == 0) {
    return data_type::type_short;
  } else if (strncmp(type, "BIGINT", 6) == 0) {
    return data_type::type_long_long;
  } else if (strcmp(type, "BOOLEAN") == 0) {
    return data_type::type_bool;
  } else if (strcmp(type, "REAL") == 0 || strcmp(type, "DOUBLE") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "FLOAT") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "DATE") == 0) {
    return data_type::type_date;
  } else if (strcmp(type, "DATETIME") == 0 || strcmp(type, "DATETIME2") == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
  } else {
    return data_type::type_unknown;
  }
}

static data_type type2data_type(SQLSMALLINT type, size_t size);

utils::result<std::vector<sql::column_definition>, sql::sql_error> odbc_connection::describe(const std::string &table)
{
  SQLHANDLE stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_.handle(), &stmt);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_DBC, connection_.handle()));
  }

  SQLCHAR buf[256];
#ifdef _MSC_VER
  strcpy_s(reinterpret_cast<char*>(buf), 256, table.c_str());
#else
  strcpy((char *) buf, table.c_str());
#endif
  ret = SQLColumns(stmt, nullptr, 0, nullptr, 0, buf, SQL_NTS, nullptr, 0);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }

  // bind to columns we need (column name, data type of column and index)
  size_t pos(0);
  size_t size(0);

  SQLCHAR column[64];
  SQLSMALLINT data_type(0);
  SQLCHAR type[64];
  SQLINTEGER not_null(0);
  SQLLEN indicator[7];
  SQLSMALLINT sql_data_type{0};
  // column name
  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column, sizeof(column), &indicator[0]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // data type
  ret = SQLBindCol(stmt, 5, SQL_C_SSHORT, &data_type, sizeof(data_type), &indicator[1]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // type name
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type, sizeof(type), &indicator[2]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // size
  ret = SQLBindCol(stmt, 7, SQL_C_SLONG, &size, sizeof(size), &indicator[3]);
  if (!is_succeeded_or_no_data(ret)) {
     return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // nullable
  ret = SQLBindCol(stmt, 11, SQL_C_SSHORT, &not_null, 0, &indicator[4]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // sql data type
  ret = SQLBindCol(stmt, 14, SQL_C_SSHORT, &sql_data_type, 0, &indicator[5]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }
  // index (1 based)
  ret = SQLBindCol(stmt, 17, SQL_C_SLONG, &pos, 0, &indicator[6]);
  if (!is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }

  std::vector<sql::column_definition> prototype;
  /* Fetch the data */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    prototype.emplace_back(std::string(reinterpret_cast<char*>(column)),
          type2data_type(data_type, size),
          size,
          not_null == SQL_NO_NULLS ? sql::null_option::NOT_NULL : sql::null_option::NULLABLE,
          reinterpret_cast<size_t>(indicator)
          );
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return utils::ok(prototype);
}

data_type type2data_type(SQLSMALLINT type, size_t size) {
  switch (type) {
    case SQL_CHAR:
      return data_type::type_char;
    case SQL_SMALLINT:
    case SQL_INTEGER:
      return data_type::type_int;
    case SQL_BIGINT:
    case SQL_NUMERIC:
      return data_type::type_long_long;
    case SQL_TYPE_DATE:
    case -9:
      if (size == 10) {
          return data_type::type_date;
      }
      if (size == 27) {
          return data_type::type_time;
      }
      return data_type::type_date;
    case SQL_TYPE_TIMESTAMP:
      return data_type::type_time;
    case SQL_VARCHAR:
      return data_type::type_varchar;
    case SQL_REAL:
      return data_type::type_float;
    case SQL_FLOAT:
      return data_type::type_double;
    case SQL_BIT:
      return data_type::type_bool;
    case SQL_LONGVARCHAR:
      return (size != 2147483647 ? data_type::type_varchar : data_type::type_text);
    case SQL_LONGVARBINARY:
      return data_type::type_blob;
    case SQL_UNKNOWN_TYPE:
    default:
      return data_type::type_unknown;
  }
}

utils::result<bool, sql::sql_error> odbc_connection::exists(const std::string &schema_name, const std::string &table_name) {
  const std::string sql("SELECT TOP 1 COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_CATALOG='" + info().database + "' AND TABLE_NAME='" + table_name + "'");

  const auto stmt = execute_statement(sql);
  if (!stmt.is_ok()) {
    return utils::error(stmt.err());
  }

  odbc_result_reader reader(*stmt);

  if (const auto fetched = reader.fetch(); !fetched.is_ok() || !*fetched) {
    return utils::ok(false);
  }

  int v{};
  reader.read_value("COUNT(*)", 1, v);

  return utils::ok(v == 1);
}

std::string odbc_connection::to_escaped_string( const utils::blob& value ) const
{
    return utils::to_string(value);
}

version odbc_connection::client_version() const {
  return {
    static_cast<unsigned int>(((ODBCVER & 0xF000) >> 12) * 10 + ((ODBCVER & 0x0F00) >> 8)),
    static_cast<unsigned int>(((ODBCVER & 0xF0) >> 4) * 10 + (ODBCVER & 0x0F)),
    0
  };
}

version odbc_connection::server_version() const {
  SQLCHAR dbms_ver[256];
  const auto ret = SQLGetInfo(connection_.handle(), SQL_DBMS_VER, (SQLPOINTER) dbms_ver, sizeof(dbms_ver), nullptr);

  if (ret == SQL_INVALID_HANDLE) {
    throw std::logic_error("odbc error (odbc) not connected");
  }

  if (!is_succeeded_or_no_data(ret)) {
    make_error(sql::sql_error_code::FAILURE, ret, SQL_HANDLE_DBC, connection_.handle());
  }

  return version::from_string(reinterpret_cast<char const *>(dbms_ver));
}

utils::result<SQLHANDLE, sql::sql_error> odbc_connection::execute_statement(const std::string &sql) const {
  if (!connection_.is_valid()) {
    return utils::error(sql::sql_error{sql::sql_error_code::FAILURE, "", "mssql no odbc connection established", "odbc"});
  }
  // create statement handle
  SQLHANDLE handle;

  if (const auto ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_.handle(), &handle); !is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, ret, SQL_HANDLE_DBC, connection_.handle(), sql));
  }

  if (const auto ret = SQLExecDirectA(handle, (SQLCHAR*) sql.c_str(), SQL_NTS); !is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::EXECUTE_FAILED, ret, SQL_HANDLE_STMT, handle, sql));
  }

  return utils::ok(handle);
}

utils::result<sql::column_definition, sql::sql_error> odbc_connection::describe_column(SQLHANDLE stmt, const SQLSMALLINT index) {
  SQLCHAR name[64];
  SQLSMALLINT name_length{};
  SQLSMALLINT data_type(0);
  SQLULEN data_size(0);
  SQLSMALLINT digits(0);
  SQLSMALLINT nullable(0);

  if (const auto ret = SQLDescribeCol(stmt, index, name, 64, &name_length, &data_type, &data_size, &digits, &nullable); !is_succeeded_or_no_data(ret)) {
    return utils::error(make_error(sql::sql_error_code::DESCRIBE_FAILED, ret, SQL_HANDLE_STMT, stmt));
  }

  return utils::ok(sql::column_definition{
    std::string(reinterpret_cast<char *>(name)),
    type2data_type(data_type, data_size),
    data_size,
    nullable == SQL_NO_NULLS ? sql::null_option::NOT_NULL : sql::null_option::NULLABLE,
    static_cast<size_t>(index)
  });
}

}

extern "C" {
MATADOR_ODBC_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info) {
  return new matador::backends::odbc::odbc_connection(info);
}

MATADOR_ODBC_API void destroy_database(matador::sql::connection_impl *db) {
  delete db;
}
}
