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

#include "database/sqlite/sqlite_statement.hpp"
#include "database/sqlite/sqlite_database.hpp"
#include "database/sqlite/sqlite_exception.hpp"

#include "database/row.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>
#include <sqlite3.h>

using std::cout;

namespace oos {

namespace sqlite {

void throw_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql = "")
{
  if (ec == SQLITE_OK) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << sqlite3_errmsg(db) << "(" << sql << ")";
  throw sqlite_exception(msg.str()); 
}

sqlite_statement::sqlite_statement(sqlite_database &db)
  : stmt_(0)
  , db_(db)
{
}

sqlite_statement::~sqlite_statement()
{
  finalize();
}

bool sqlite_statement::fetch()
{

//  std::cerr << "executing " << sql() << "\n";

  int ret = sqlite3_step(stmt_);
  if (ret == SQLITE_ROW) {
    // retrieved new row
    // create row object
    return true;
  } else if (ret == SQLITE_DONE) {
    // no further row available
    return false;
  } else {
    // error, throw exception
    const char *msg = sqlite3_errmsg(db_());
    throw sqlite_exception(std::string("sqlite3_step: ") + msg);
  }
  return false;
}

void sqlite_statement::prepare(const std::string &sql, int params, int results)
{
  // destroy statement
  finalize();
  // set new sql statement
  statement::prepare(sql, params, results);
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_(), sql.c_str(), sql.size(), &stmt_, 0);
  throw_error(ret, db_(), "sqlite3_prepare_v2", sql);
}

void sqlite_statement::reset(bool clear_bindings)
{
  sqlite3_reset(stmt_);
  if (clear_bindings) {
    sqlite3_clear_bindings(stmt_);
  }
}

int sqlite_statement::finalize()
{
  if (!stmt_) {
    return SQLITE_OK;
  }
//  std::cout << "finalizing statement 0x" << std::hex << stmt_ << "\n";
  int ret = sqlite3_finalize(stmt_);
  throw_error(ret, db_(), "sqlite3_finalize");
  stmt_ = 0;
  return ret;
}

int sqlite_statement::column_count() const
{
  return sqlite3_column_count(stmt_);
}

const char* sqlite_statement::column_name(int i) const
{
  return sqlite3_column_name(stmt_, i);
}

void sqlite_statement::column(int i, bool &value)
{
  value = sqlite3_column_int(stmt_, i) > 0;
}

void sqlite_statement::column(int i, char &value)
{
  value = (char)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, float &value)
{
  value = (float)sqlite3_column_double(stmt_, i);
}

void sqlite_statement::column(int i, double &value)
{
  value = sqlite3_column_double(stmt_, i);
}

void sqlite_statement::column(int i, short &value)
{
  value = (short)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, int &value)
{
  value = sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, long &value)
{
  value = (long)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, unsigned char &value)
{
  value = (unsigned char)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, unsigned short &value)
{
  value = (unsigned short)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, unsigned int &value)
{
  value = (unsigned int)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, unsigned long &value)
{
  value = (unsigned long)sqlite3_column_int(stmt_, i);
}

void sqlite_statement::column(int i, char *value, int &len)
{
  int s = sqlite3_column_bytes(stmt_, i);

  if (s > len) {
    throw sqlite_exception("unsufficient size of character buffer");
  } else {
    const char *res = (const char*)sqlite3_column_text(stmt_, i);
    len = strlen(res);
    strncpy(value, res, len);
    value[len] = '\0';
  }
}

void sqlite_statement::column(int i, std::string &value)
{
  int s = sqlite3_column_bytes(stmt_, i);
  value.assign((const char*)sqlite3_column_text(stmt_, i), s);
}

int sqlite_statement::column_type(int i) const
{
  return sqlite3_column_type(stmt_, i);
}

int sqlite_statement::bind(int i, double value)
{
  int ret = sqlite3_bind_double(stmt_, i, value);
  throw_error(ret, db_(), "sqlite3_bind_double");
  return ret;
}

int sqlite_statement::bind(int i, int value)
{
  int ret = sqlite3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int sqlite_statement::bind(int i, long value)
{
  int ret = sqlite3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int sqlite_statement::bind(int i, unsigned int value)
{
  int ret = sqlite3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int sqlite_statement::bind(int i, unsigned long value)
{
  int ret = sqlite3_bind_int(stmt_, i, value);
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int sqlite_statement::bind(int i, const char *value, int len)
{
  int ret = sqlite3_bind_text(stmt_, i, value, len, 0);
  throw_error(ret, db_(), "sqlite3_bind_text");
  return ret;
}

int sqlite_statement::bind(int i, const std::string &value)
{
  int ret = sqlite3_bind_text(stmt_, i, value.c_str(), value.size(), 0);
  throw_error(ret, db_(), "sqlite3_bind_text");
  return ret;
}

int sqlite_statement::bind_null(int i)
{
  int ret = sqlite3_bind_null(stmt_, i);
  throw_error(ret, db_(), "sqlite3_bind_null");
  return ret;
}

database& sqlite_statement::db()
{
  return db_;
}

const database& sqlite_statement::db() const
{
  return db_;
}

}

}
