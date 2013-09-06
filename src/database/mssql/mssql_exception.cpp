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

#include "database/mssql/mssql_exception.hpp"

#include <sql.h>
#include <sqlext.h>

#include <string>
#include <sstream>

namespace oos {

namespace mssql {

std::string error_message(const std::string &source, const std::string &sql)
{
  std::stringstream msg;
  msg << source << ": " << " (" << sql << ")";
  return msg.str();
}

void throw_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &msg)
{
  if (SQL_SUCCEEDED(ret)) {
    return;
  }
  SQLCHAR state[6];
  SQLINTEGER error;
  SQLCHAR data[512];
  SQLSMALLINT over_by;

  ret = SQL_ERROR;
  SQLSMALLINT i = 0;
  std::stringstream text;
  do {
    ret = SQLGetDiagRec(htype, hndl, ++i, state, &error, data, 511, &over_by);
    if (ret == SQL_SUCCESS) {
      text << "odbc error [" << state << "] " << i << " (" << error << "): " << data;
    }
  } while (ret == SQL_SUCCESS);

  throw mssql_exception(source, msg);
}

void throw_error(const std::string &source, const std::string &sql)
{
  throw mssql_exception(source, sql);
}

mssql_exception::mssql_exception(const std::string &source, const std::string &what)
  : database_exception("mssql", (source + ": " + what).c_str())
{}

mssql_exception::~mssql_exception() throw()
{}

mssql_stmt_exception::mssql_stmt_exception(const std::string &what)
  : database_exception("mssql", what.c_str())
{}

mssql_stmt_exception::mssql_stmt_exception(const std::string &source, const std::string &what)
  : database_exception("mssql", error_message(source, what).c_str())
{}

mssql_stmt_exception::~mssql_stmt_exception() throw()
{}

}

}
