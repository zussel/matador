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

#include "mysql_exception.hpp"

#include <string>
#include <sstream>

namespace oos {

namespace mysql {

std::string error_message(MYSQL *db, const std::string &source, const std::string &sql)
{
  std::stringstream msg;
  msg << source << ": " << mysql_error(db) << " (" << sql << ")";
  return msg.str();
}

std::string error_message(MYSQL_STMT *stmt, const std::string &source, const std::string &sql)
{
  std::stringstream msg;
  msg << source << ": " << mysql_stmt_error(stmt) << " (" << sql << ")";
  return msg.str();
}

void throw_error(const std::string &source, const std::string &msg)
{
  throw mysql_exception(source, msg);
}

void throw_error(int ec, MYSQL *db, const std::string &source, const std::string &sql)
{
  if (ec == 0) {
    return;
  }
  throw mysql_exception(db, source, sql);
}

void throw_stmt_error(int ec, MYSQL_STMT *stmt, const std::string &source, const std::string &sql)
{
  if (ec == 0) {
    return;
  }
  throw mysql_stmt_exception(stmt, source, sql); 
}

mysql_exception::mysql_exception(const std::string &source, const std::string &what)
  : sql_exception("mysql", (source + ": " + what).c_str())
{}


mysql_exception::mysql_exception(MYSQL *db, const std::string &source, const std::string &what)
  : sql_exception("mysql", error_message(db, source, what).c_str())
{}

mysql_exception::~mysql_exception() throw()
{}

mysql_stmt_exception::mysql_stmt_exception(const std::string &what)
  : sql_exception("mysql", what.c_str())
{}

mysql_stmt_exception::mysql_stmt_exception(MYSQL_STMT *stmt, const std::string &source, const std::string &what)
  : sql_exception("mysql", error_message(stmt, source, what).c_str())
{}

mysql_stmt_exception::~mysql_stmt_exception() throw()
{}

}

}
