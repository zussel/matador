#include "odbc_connection.hpp"
#include "odbc_error.hpp"
#include "odbc_result_reader.hpp"
#include "odbc_statement.hpp"

#include "matador/sql/record.hpp"

#include "matador/utils/data_types.hpp"

#include <cstring>
#include <memory>
#include <utility>

#include <sql.h>
#include <sqlext.h>

namespace matador::backends::odbc {

odbc_connection::odbc_connection(const sql::connection_info &info)
: connection_impl(info) {
}

void odbc_connection::open()
{
  if (is_open()) {
    return;
  }

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &odbc_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_odbc_error(ret, SQL_HANDLE_ENV, odbc_, "odbc");
  }

  ret = SQLSetEnvAttr(odbc_, SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3, 0);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_odbc_error(ret, SQL_HANDLE_ENV, odbc_, "odbc");
  }

  ret = SQLAllocHandle(SQL_HANDLE_DBC, odbc_, &connection_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc");
  }

  SQLSetConnectAttr(connection_, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);

  std::string dns("DRIVER={" + info().driver + "};SERVER=" + info().hostname + ";Protocol=TCPIP;Port=" + std::to_string(info().port) + ";DATABASE=" + info().database + ";UID=" + info().user + ";PWD=" + info().password + ";");

  SQLCHAR retconstring[1024];
  ret = SQLDriverConnect(connection_, nullptr, (SQLCHAR*)dns.c_str(), SQL_NTS, retconstring, 1024, nullptr, SQL_DRIVER_NOPROMPT);

  throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc");
}

void odbc_connection::close()
{
  if (!is_open()) {
    return;
  }

  SQLRETURN ret = SQLDisconnect(connection_);

  throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc");

  ret = SQLFreeHandle(SQL_HANDLE_DBC, connection_);
  throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc");

  connection_ = nullptr;

  ret = SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
  throw_odbc_error(ret, SQL_HANDLE_ENV, odbc_, "odbc");

  odbc_ = nullptr;
}

bool odbc_connection::is_open() const
{
  return connection_ != nullptr;
}

bool odbc_connection::is_valid() const
{
  SQLUINTEGER connectionDead;
  SQLRETURN ret = SQLGetConnectAttr(connection_, SQL_ATTR_CONNECTION_DEAD, &connectionDead, 0, NULL);
  return (ret == SQL_SUCCESS && connectionDead == SQL_CD_FALSE);
}

int odbc_connection::parse_result(void* param, int column_count, char** values, char** columns)
{
  auto *context = static_cast<fetch_context*>(param);

  odbc_result_reader::columns column;
  for(int i = 0; i < column_count; ++i) {
    // copy and store column data;
    if (values[i] == nullptr) {
      auto val = new char[1];
      val[0] = '\0';
      column.push_back(val);
    } else {
      size_t size = strlen(values[i]);
      auto val = new char[size + 1];
      std::memcpy(val, values[i], size);
      val[size] = '\0';
      column.push_back(val);
    }
  }
  context->rows.emplace_back(column);

  if (context->prototype.empty()) {
    for(int i = 0; i < column_count; ++i) {
      context->prototype.emplace_back(columns[i]);
    }
  }

  return 0;
}

odbc_connection::fetch_context odbc_connection::fetch_internal(const std::string &stmt)
{
  fetch_context context;
  char *errmsg = nullptr;
  const int ret = sqlite3_exec(con, stmt.c_str(), parse_result, &context, &errmsg);

  throw_odbc_error(ret, db_, "sqlite", stmt);

  return context;
}

size_t odbc_connection::execute(const std::string &stmt)
{
  if (!connection_) {
    throw std::logic_error("mssql no odbc connection established");
  }
  // create statement handle
  SQLHANDLE handle;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &handle);
  throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc", stmt);

  ret = SQLExecDirectA(handle, (SQLCHAR*)stmt.c_str(), SQL_NTS);

  throw_odbc_error(ret, SQL_HANDLE_STMT, handle, "odbc", stmt);

  return 0;
}

std::unique_ptr<sql::query_result_impl> odbc_connection::fetch(const std::string &stmt)
{
  auto context = fetch_internal(stmt);

  return std::move(std::make_unique<sql::query_result_impl>(std::make_unique<odbc_result_reader>(std::move(context.rows), context.prototype.size()), std::move(context.prototype)));
}

std::unique_ptr<sql::statement_impl> odbc_connection::prepare(sql::query_context query)
{
  sqlite3_stmt *stmt{};
  int ret = sqlite3_prepare_v2(db_, query.sql.c_str(), static_cast<int>(query.sql.size()), &stmt, nullptr);
  throw_odbc_error(ret, db_, "sqlite3_prepare_v2", query.sql);

  return std::make_unique<odbc_statement>(db_, stmt, query);
}

data_type string2type(const char *type)
{
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
  } else if (strcmp(type, "REAL") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "FLOAT") == 0) {
    return data_type::type_float;
  } else if (strcmp(type, "DOUBLE") == 0) {
    return data_type::type_double;
  } else if (strcmp(type, "BLOB") == 0) {
    return data_type::type_blob;
  } else if (strcmp(type, "NULL") == 0) {
    return data_type::type_null;
  } else if (strncmp(type, "VARCHAR", 7) == 0) {
    return data_type::type_varchar;
  } else if (strcmp(type, "DATE") == 0) {
    return data_type::type_date;
  } else if (strcmp(type, "DATETIME") == 0) {
    return data_type::type_time;
  } else if (strcmp(type, "TEXT") == 0) {
    return data_type::type_text;
  } else {
    return data_type::type_unknown;
  }
}

static data_type type2data_type(SQLSMALLINT type, size_t size);

std::vector<sql::column_definition> odbc_connection::describe(const std::string& table)
{
  SQLHANDLE stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &stmt);
  throw_odbc_error(ret, SQL_HANDLE_DBC, connection_, "odbc");

  SQLCHAR buf[256];
#ifdef _MSC_VER
  strcpy_s((char*)buf, 256, table.c_str());
#else
  strcpy((char*)buf, table.c_str());
#endif
  ret = SQLColumns(stmt, nullptr, 0, nullptr, 0, buf, SQL_NTS, nullptr, 0);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");

  // bind to columns we need (column name, data type of column and index)
  size_t pos(0);
  size_t size(0);

  SQLCHAR column[64];
  SQLSMALLINT data_type(0);
  SQLCHAR type[64];
  SQLINTEGER not_null(0);
  SQLLEN indicator[6];

  // column name
  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column, sizeof(column), &indicator[0]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
  // data type
  ret = SQLBindCol(stmt, 5, SQL_C_SSHORT, &data_type, sizeof(data_type), &indicator[1]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
  // type name
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type, sizeof(type), &indicator[2]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
  // size
  ret = SQLBindCol(stmt, 7, SQL_C_SLONG, &size, sizeof(size), &indicator[3]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
  // nullable
  ret = SQLBindCol(stmt, 11, SQL_C_SSHORT, &not_null, 0, &indicator[4]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
  // index (1 based)
  ret = SQLBindCol(stmt, 17, SQL_C_SLONG, &pos, 0, &indicator[5]);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");

  std::vector<sql::column_definition> prototype;
  /* Fetch the data */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    prototype.emplace_back(std::string((char*)column), type2data_type(data_type, size), utils::null_attributes, not_null == 0, pos - 1);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return prototype;
}

data_type type2data_type(SQLSMALLINT type, size_t size)
{
  switch (type) {
    case SQL_CHAR:
      return data_type::type_char;
    case SQL_SMALLINT:
      return data_type::type_int;
    case SQL_INTEGER:
      return data_type::type_int;
    case SQL_BIGINT:
    case SQL_NUMERIC:
      return data_type::type_long_long;
    case SQL_TYPE_DATE:
    case -9:
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
    case SQL_UNKNOWN_TYPE:
    default:
      return data_type::type_unknown;
  }
}

bool odbc_connection::exists(const std::string &schema_name, const std::string &table_name)
{
  const auto result = fetch_internal("SELECT COUNT(*) FROM odbc_master WHERE type='table' AND tbl_name='" + table_name + "' LIMIT 1");
  odbc_result_reader reader(result.rows, result.prototype.size());

  if (!reader.fetch()) {
    // Todo: throw an exception?
    return false;
  }

  int v{};
  reader.read_value(nullptr, 0, v);

  return v == 1;
}

}

extern "C"
{
MATADOR_ODBC_API matador::sql::connection_impl *create_database(const matador::sql::connection_info &info)
{
  return new matador::backends::sqlite::odbc_connection(info);
}

MATADOR_ODBC_API void destroy_database(matador::sql::connection_impl *db)
{
  delete db;
}

}
