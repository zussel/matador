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

#include "mssql_database.hpp"
#include "mssql_statement.hpp"
#include "mssql_result.hpp"
#include "mssql_types.hpp"
#include "mssql_exception.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/database_sequencer.hpp"
#include "database/row.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include "tools/string.hpp"

#include <stdexcept>
#include <sstream>

#include <sql.h>
#include <sqlext.h>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  
namespace mssql {
  
mssql_database::mssql_database(session *db)
  : database(db, new database_sequencer(*this))
  , odbc_(0)
  , connection_(0)
  , is_open_(false)
  , retries_(1)
{
}

mssql_database::~mssql_database()
{
  close();
}


void mssql_database::on_open(const std::string &connection)
{
  // parse user[:passwd]@host/db ([Drivername])
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
  pos = con.find('/');
  std::string host = con.substr(0, pos);
  std::string db = con.substr(pos + 1);

  // get driver
  pos = db.find('(');
  if (pos != std::string::npos) {
    driver = db.substr(pos+1);
    db = db.substr(0, pos);
    db = trim(db);
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

  std::string dns("DRIVER={" + driver + "};SERVER=" + host + "\\" + db + ";DATABASE=test;UID=" + user + ";PWD=sascha;");

  SQLCHAR retconstring[1024];
  ret = SQLDriverConnect(connection_, 0, (SQLCHAR*)dns.c_str(), SQL_NTS, retconstring, 1024, NULL,SQL_DRIVER_NOPROMPT);

  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on connect");

  is_open_ = true;
}

bool mssql_database::is_open() const
{
  return is_open_;
}

void mssql_database::on_close()
{
  SQLRETURN ret = SQLDisconnect(connection_);

  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on close");

  ret = SQLFreeHandle(SQL_HANDLE_DBC, connection_);
  throw_error(ret, SQL_HANDLE_DBC, connection_, "mssql", "error on freeing connection");
 
  ret = SQLFreeHandle(SQL_HANDLE_ENV, odbc_);
  throw_error(ret, SQL_HANDLE_ENV, odbc_, "mssql", "error on freeing odbc");
 
  is_open_ = false;
}

result* mssql_database::on_execute(const std::string &sqlstr)
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

  return new mssql_result(stmt, true);
}

result* mssql_database::create_result()
{
  return 0;
}

statement* mssql_database::create_statement()
{
  return new mssql_statement(*this);
}

void mssql_database::on_begin()
{
  result *res = execute("BEGIN TRANSACTION;");
  // TODO: check result
  delete res;
}

void mssql_database::on_commit()
{
  result *res = execute("COMMIT;");
  // TODO: check result
  delete res;
}

void mssql_database::on_rollback()
{
  result *res = execute("ROLLBACK;");
  // TODO: check result
  delete res;
}

const char* mssql_database::type_string(data_type_t type) const
{
  switch(type) {
    case type_char:
      return "SMALLINT";
    case type_short:
      return "SMALLINT";
    case type_int:
      return "INT";
    case type_long:
      return "BIGINT";
    case type_unsigned_char:
      return "SMALLINT";
    case type_unsigned_short:
      return "INT";
    case type_unsigned_int:
      return "BIGINT";
    case type_unsigned_long:
      return "NUMERIC(21,0)";
    case type_bool:
      return "BIT";
    case type_float:
      return "FLOAT";
    case type_double:
      return "FLOAT";
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
      return "TEXT";
    default:
      {
        std::stringstream msg;
        msg << "mssql database: unknown type xxx [" << type << "]";
        throw std::logic_error(msg.str());
        //throw std::logic_error("mssql database: unknown type");
      }
    }
}

SQLHANDLE mssql_database::operator()()
{
  return connection_;
}

}

}

extern "C"
{
  OOS_MSSQL_API oos::database* create_database(oos::session *ses)
  {
    return new oos::mssql::mssql_database(ses);
  }

  OOS_MSSQL_API void destroy_database(oos::database *db)
  {
    delete db;
  }
}
