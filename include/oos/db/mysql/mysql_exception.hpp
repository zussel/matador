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

#ifndef MYSQL_EXCEPTION_HPP
#define MYSQL_EXCEPTION_HPP

#ifdef _MSC_VER
  #ifdef oos_mysql_EXPORTS
    #define OOS_MYSQL_API __declspec(dllexport)
  #else
    #define OOS_MYSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MYSQL_API
#endif

#include "oos/sql/sql_exception.hpp"

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace oos {

namespace mysql {

void throw_error(const std::string &source, const std::string &sql = "");

void throw_error(int ec, MYSQL *db, const std::string &source, const std::string &sql = "");

void throw_stmt_error(int ec, MYSQL_STMT *stmt, const std::string &source, const std::string &sql = "");

class mysql_exception : public sql_exception
{
public:
  mysql_exception(const std::string &source, const std::string &what);
  mysql_exception(MYSQL *db, const std::string &source, const std::string &what);

  virtual ~mysql_exception() throw();
};

class mysql_stmt_exception : public sql_exception
{
public:
  explicit mysql_stmt_exception(const std::string &what);
  mysql_stmt_exception(MYSQL_STMT *stmt, const std::string &source, const std::string &what);
  
  virtual ~mysql_stmt_exception() throw();
};

}

}

#endif /* MYSQL_EXCEPTION_HPP */
