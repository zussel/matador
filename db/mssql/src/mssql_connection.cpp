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

#include "mssql_connection.hpp"
#include "mssql_statement.hpp"
#include "mssql_result.hpp"
#include "mssql_types.hpp"
#include "mssql_exception.hpp"

#include "tools/string.hpp"

using namespace std::placeholders;

namespace oos {
  
namespace mssql {

mssql_connection::mssql_connection()
  : odbc_(0)
  , connection_(0)
  , is_open_(false)
{
}

mssql_connection::~mssql_connection()
{
  close();
}


void mssql_connection::open(const std::string &connection)
{
  // parse user[:passwd]@host/instance/db ([Drivername])
  std::string con = connection;
  std::string::size_type pos = con.find('@');
  std::string user, passwd;
  std::string driver;
//  bool has_pwd = true;
  if (pos == std::string::npos) {
	  throw_error("mssql:open", "invalid dsn: " + connection);
  } else {
    // try to find colon (:)
    std::string credentials = con.substr(0, pos);
    std::string::size_type colpos = credentials.find(':');
    if (colpos != std::string::npos) {
      // found colon, extract user and passwd
      user = credentials.substr(0, colpos);
      passwd = credentials.substr(colpos + 1, pos);
    } else {
      // only user name given
      user = credentials.substr(0, pos);
    }
  }
  // get connection part
  con = con.substr(pos + 1);
  pos = con.find_last_of('/');
  std::string host = con.substr(0, pos);
  db_ = con.substr(pos + 1);

  // get driver
  pos = db_.find('(');
  if (pos != std::string::npos) {
    driver = db_.substr(pos+1);
    db_ = db_.substr(0, pos);
    db_ = trim(db_);
    pos = driver.find(')');
    driver = driver.substr(0, pos);
  } else {
    driver = "SQL Server";
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

  std::string dns("DRIVER={" + driver + "};SERVER=" + host + ";Protocol=TCPIP;Port=1433;DATABASE=" + db_ + ";UID=" + user + ";PWD=" + passwd + ";");

  SQLCHAR retconstring[1024];
  ret = SQLDriverConnect(connection_, 0, (SQLCHAR*)dns.c_str(), SQL_NTS, retconstring, 1024, NULL,SQL_DRIVER_NOPROMPT);

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

oos::detail::result_impl *mssql_connection::execute(const oos::sql &sql)
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

oos::detail::statement_impl *mssql_connection::prepare(const oos::sql &stmt)
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
  SQLColumns(stmt, NULL, 0, NULL, 0, buf, SQL_NTS, NULL, 0);
  //std::unique_ptr<mssql_result> res(static_cast<mssql_result*>(execute(stmt)));

  // bind to columns we need (column name, data type of column and index)
  SQLINTEGER pos(0);
  SQLCHAR column[64];
  SQLCHAR type[64];
  SQLINTEGER not_null(0);
  SQLLEN indicator[4];

  ret = SQLBindCol(stmt, 4, SQL_C_CHAR, column, sizeof(column), &indicator[0]);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on sql columns statement (column)");
  ret = SQLBindCol(stmt, 6, SQL_C_CHAR, type, sizeof(type), &indicator[1]);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on sql columns statement (type)");
  ret = SQLBindCol(stmt, 11, SQL_C_LONG, &not_null, 0, &indicator[2]);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on sql columns statement (not null)");
  ret = SQLBindCol(stmt, 17, SQL_C_LONG, &pos, 0, &indicator[3]);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on sql columns statement (pos)");

  std::vector<field> fields;

  /* Fetch the data */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    field f;
    f.index(pos - 1);
    f.name(std::string((char*)column));
    f.type(dialect_.string_type((char*)type));
    f.not_null(not_null == 0);

    fields.push_back(f);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return fields;
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
  OOS_MSSQL_API oos::connection_impl* create_database()
  {
    return new oos::mssql::mssql_connection();
  }

  OOS_MSSQL_API void destroy_database(oos::connection_impl *db)
  {
    delete db;
  }
}
