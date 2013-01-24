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

#include "database/mysql/mysql_statement.hpp"
#include "database/mysql/mysql_database.hpp"
#include "database/mysql/mysql_exception.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <mysql/mysql.h>

namespace oos {

namespace mysql {

void throw_error(int ec, st_mysql *db, const std::string &source, const std::string &sql = "")
{
  if (ec == 0) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << mysql_error(db) << "(" << sql << ")";
  throw mysql_exception(msg.str()); 
}

mysql_statement::mysql_statement(mysql_database &db)
  : stmt_(0)
  , db_(db)
{
}

mysql_statement::~mysql_statement()
{
  finalize();
}

bool mysql_statement::step()
{

//  std::cerr << "executing " << sql() << "\n";

  int ret = mysql_stmt_next_result(stmt_);
  if (ret == 0/*SQLITE_ROW*/) {
    // retrieved new row
    // create row object
    return true;
  } else if (ret == 0/*SQLITE_DONE*/) {
    // no further row available
    return false;
  } else {
    // error, throw exception
    throw_error(ret, db_(), "mysql_stmt_next_result");
  }
  return false;
}

void mysql_statement::prepare(const std::string &sql)
{
  // destroy statement
  finalize();
  // set new sql statement
  statement::prepare(sql);
  // create statement
  stmt_ = mysql_stmt_init(db_());
  // prepare mysql statement
  int ret = mysql_stmt_prepare(stmt_, sql.c_str(), sql.size());
  throw_error(ret, db_(), "mysql_stmt_prepare", sql);
}

void mysql_statement::reset(bool clear_bindings)
{
  mysql_stmt_reset(stmt_);
  /*
  if (clear_bindings) {
    mysql3_clear_bindings(stmt_);
  }
  */
}

int mysql_statement::finalize()
{
  if (!stmt_) {
    return 0/*SQLITE_OK*/;
  }
//  std::cout << "finalizing statement 0x" << std::hex << stmt_ << "\n";
//  int ret = mysql3_finalize(stmt_);
//  throw_error(ret, db_(), "mysql3_finalize");
  /*
  if (ret != SQLITE_OK) {
    std::string msg(mysql3_errmsg(db_()));
    throw mysql_exception("mysql3_finalize: " + msg);
  }
  */
  stmt_ = 0;
  return ret;
}

int mysql_statement::column_count() const
{
  return mysql_stmt_field_count(stmt_);
}

const char* mysql_statement::column_name(int i) const
{
  return mysql3_column_name(stmt_, i);
}

const char* mysql_statement::column_text(int i, int &s) const
{
  s = mysql3_column_bytes(stmt_, i);
  return (const char*)mysql3_column_text(stmt_, i);
}

int mysql_statement::column_int(int i) const
{
  return mysql3_column_int(stmt_, i);
}

double mysql_statement::column_double(int i) const
{
  return mysql3_column_double(stmt_, i);
}

int mysql_statement::column_type(int i) const
{
  return mysql3_column_type(stmt_, i);
}

int mysql_statement::bind(int i, double value)
{
  int ret = mysql3_bind_double(stmt_, i, value);
  throw_error(ret, db_(), "mysql_bind_double");
  return ret;
}

int mysql_statement::bind(int i, int value)
{
  int ret = mysql3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "mysql_bind_int");
  return ret;
}

int mysql_statement::bind(int i, long value)
{
  int ret = mysql3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "mysql_bind_int");
  return ret;
}

int mysql_statement::bind(int i, unsigned int value)
{
  int ret = mysql3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "mysql_bind_int");
  return ret;
}

int mysql_statement::bind(int i, const char *value, int len)
{
  int ret = mysql3_bind_text(stmt_, i, value, len, 0);
  throw_error(ret, db_(), "mysql_bind_text");
  return ret;
}

int mysql_statement::bind_null(int i)
{
  int ret = mysql3_bind_null(stmt_, i);
  throw_error(ret, db_(), "mysql_bind_null");
  return ret;
}

database& mysql_statement::db()
{
  return db_;
}

const database& mysql_statement::db() const
{
  return db_;
}

}

}
