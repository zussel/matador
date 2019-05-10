/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "matador/db/mssql/mssql_connection.hpp"
#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_result.hpp"
#include "matador/db/mssql/mssql_types.hpp"
#include "matador/db/mssql/mssql_exception.hpp"

#include "matador/utils/string.hpp"

#include <regex>

using namespace std::placeholders;

namespace matador {
  
namespace mssql {

mssql_connection::mssql_connection()
  : odbc_(nullptr)
  , connection_(nullptr)
  , is_open_(false)
{
}

mssql_connection::~mssql_connection()
{
  close();
}


void mssql_connection::open(const std::string &connection)
{
  // dns syntax:
  // user[:passwd]@host[:port]/instance/db [(Drivername)]
  //static const std::regex DNS_RGX("(.+?)(?::(.+?))?@([^:]+?)(?::([1-9][0-9]*?))?(?:/(.+?))?/(.+?)(?:\\s+\\((.+)\\))?");
  static const std::regex DNS_RGX(R"((.+?)(?::(.+?))?@([^:]+?)(?::([1-9][0-9]*?))?(?:/(.+?))?/(.+?)(?:\s+\((.+)\))?)");
  std::smatch what;

  if (!std::regex_match(connection, what, DNS_RGX)) {
    throw_error("mssql:open", "invalid dns: " + connection);
  }

  const std::string user = what[1].str();
  const std::string passwd = what[2].str();

  // get connection part
  const std::string host = what[3].str();
  std::string port = "1433";
  if (what[4].matched) {
    port = what[4].str();
  }

  // Should we just ignore the "instance" part?
  // const std::string instance = what[5].str();

  db_ = what[6].str();

  std::string driver = "SQL Server";
  if (what[7].matched) {
    driver = what[7].str();
  }

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &odbc_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_error(ret, SQL_HANDLE_ENV, odbc_, "mssql", "couldn't get odbc handle");
  }

  ret = SQLSetEnvAttr(odbc_, SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3, 0);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_error(ret, SQL_HANDLE_ENV, odbc_, "mssql", "couldn't set odbc driver version");
  }

  ret = SQLAllocHandle(SQL_HANDLE_DBC, odbc_, &connection_);
  if (ret != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
    throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "couldn't get connection handle");
  }

  SQLSetConnectAttr(connection_, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);

  std::string dns("DRIVER={" + driver + "};SERVER=" + host + ";Protocol=TCPIP;Port=" + port + ";DATABASE=" + db_ + ";UID=" + user + ";PWD=" + passwd + ";");

  SQLCHAR retconstring[1024];
  ret = SQLDriverConnect(connection_, nullptr, (SQLCHAR*)dns.c_str(), SQL_NTS, retconstring, 1024, nullptr, SQL_DRIVER_NOPROMPT);

  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on connect");

  is_open_ = true;
}

bool mssql_connection::is_open() const
{
  return is_open_;
}

void mssql_connection::close()
{
  if (!is_open_) {
    return;
  }

  SQLRETURN ret = SQLDisconnect(connection_);

  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on close");

  ret = SQLFreeHandle(SQL_HANDLE_DBC, connection_);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on freeing connection");

  connection_ = nullptr;

  ret = SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
  throw_error(ret, SQL_HANDLE_ENV, odbc_, "mssql", "error on freeing odbc");

  odbc_ = nullptr;

  is_open_ = false;
}

matador::detail::result_impl *mssql_connection::execute(const matador::sql &sql)
{
  std::string stmt(dialect_.direct(sql));
  return execute(stmt);
}

detail::result_impl* mssql_connection::execute(const std::string &sqlstr)
{
  if (!connection_) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLHANDLE stmt;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &stmt);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on creating sql statement");

  // execute statement
//  int retry = retries_;
  ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlstr.c_str(), SQL_NTS);

  /*
  do {
    ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlstr.c_str(), SQL_NTS);
  } while (retry-- && !(SQL_SUCCEEDED(ret) || SQL_NO_DATA));
  */

  throw_error(ret, SQL_HANDLE_STMT, stmt, sqlstr, "error on query execute");

  return new mssql_result(stmt);
}

matador::detail::statement_impl *mssql_connection::prepare(const matador::sql &stmt)
{
  return new mssql_statement(*this, stmt);
}

void mssql_connection::begin()
{
  execute_no_result("BEGIN TRANSACTION;");
}

void mssql_connection::commit()
{
  execute_no_result("COMMIT;");
}

void mssql_connection::rollback()
{
  execute_no_result("ROLLBACK;");
}

std::string mssql_connection::type() const
{
  return "mssql";
}

std::string mssql_connection::version() const
{
  return "1.1.1";
}

bool mssql_connection::exists(const std::string &tablename)
{
  std::string stmt("SELECT TOP 1 COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_CATALOG='" + db_ + "' AND TABLE_NAME='" + tablename + "'");
  std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute(stmt)));

  if (!res->fetch()) {
    return false;
  } else {
    return res->get<int>(1) == 1;
  }
}

static data_type type2data_type(SQLSMALLINT type, size_t size);

std::vector<field> mssql_connection::describe(const std::string &table)
{
  // create statement handle
  SQLHANDLE stmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &stmt);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on creating sql statement");

  //std::string stmt("EXEC SP_COLUMNS " + table);

  SQLCHAR buf[256];
#ifdef _MSC_VER
  strcpy_s((char*)buf, 256, table.c_str());
#else
  strcpy((char*)buf, table.c_str());
#endif
  ret = SQLColumns(stmt, nullptr, 0, nullptr, 0, buf, SQL_NTS, nullptr, 0);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on executing column description");
  //std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute(stmt)));

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
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (column)");
  // data type
  ret = SQLBindCol(stmt, 5, SQL_C_SSHORT, &data_type, sizeof(data_type), &indicator[1]);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (data type)");
  // type name
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type, sizeof(type), &indicator[2]);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (type name)");
  // size
  ret = SQLBindCol(stmt, 7, SQL_C_SLONG, &size, sizeof(size), &indicator[3]);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (data size)");
  // nullable
  ret = SQLBindCol(stmt, 11, SQL_C_SSHORT, &not_null, 0, &indicator[4]);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (not null)");
  // index (1 based)
  ret = SQLBindCol(stmt, 17, SQL_C_SLONG, &pos, 0, &indicator[5]);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "error on sql columns statement (pos)");

  std::vector<field> fields;

  /* Fetch the data */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    field f;
    f.index(pos - 1);
    f.size(size);
    f.name(std::string((char*)column));
    f.type(type2data_type(data_type, size));
    //f.type(dialect_.string_type((char*)type));
    f.not_null(not_null == 0);

    fields.push_back(f);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return fields;
}

data_type type2data_type(SQLSMALLINT type, size_t size)
{
  switch (type) {
  case SQL_CHAR:
    return data_type::type_char;
  case SQL_SMALLINT:
    return data_type::type_short;
  case SQL_INTEGER:
    return data_type::type_int;
  case SQL_BIGINT:
  case SQL_NUMERIC:
    return data_type::type_long;
  case SQL_DATE:
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

SQLHANDLE mssql_connection::handle()
{
  return connection_;
}

unsigned long mssql_connection::last_inserted_id()
{
  std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute("select scope_identity()")));
  return res->get<unsigned long>(1);
}

basic_dialect* mssql_connection::dialect()
{
  return &dialect_;
}

void mssql_connection::execute_no_result(const std::string &stmt)
{
  if (!connection_) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLHANDLE handle;

  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, connection_, &handle);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on creating sql statement");

  // execute statement
//  int retry = retries_;
  ret = SQLExecDirectA(handle, (SQLCHAR*)stmt.c_str(), SQL_NTS);

  /*
  do {
    ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlstr.c_str(), SQL_NTS);
  } while (retry-- && !(SQL_SUCCEEDED(ret) || SQL_NO_DATA));
  */

  throw_error(ret, SQL_HANDLE_STMT, handle, stmt, "error on query execute");
}

}

}

extern "C"
{
  OOS_MSSQL_API matador::connection_impl* create_database()
  {
    return new matador::mssql::mssql_connection();
  }

  OOS_MSSQL_API void destroy_database(matador::connection_impl *db)
  {
    delete db;
  }
}
