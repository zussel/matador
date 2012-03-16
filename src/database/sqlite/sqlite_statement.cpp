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

#include "database/row.hpp"

#include <stdexcept>
#include <iostream>
#include <sqlite3.h>

namespace oos {

namespace sqlite {

sqlite_statement::sqlite_statement(sqlite_database &db)
  : stmt_(0)
  , db_(db)
{
}

sqlite_statement::~sqlite_statement()
{
  finalize();
}

bool sqlite_statement::step()
{

  std::cerr << "executing " << sql() << "\n";

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
//    throw std::sqlite_error("error on calling sqlite3_step");
    const char *msg = sqlite3_errmsg(db_());
    throw std::logic_error(std::string("SQLite ERROR [sqlite3_step]: ") + msg);
  }
  return false;
}

void sqlite_statement::prepare(const std::string &sql)
{
  // destroy statement
  finalize();
  // set new sql statement
  statement_impl::prepare(sql);
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_(), sql.c_str(), sql.size(), &stmt_, 0);
  if (SQLITE_OK != ret) {
    std::string msg(sqlite3_errmsg(db_()));
    std::cout << "error while preparing statement: " << sql << " " << msg << "\n";
    throw std::runtime_error("error while preparing statement: " + sql + " " + msg);
  }
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
  if (ret != SQLITE_OK) {
    std::string msg(sqlite3_errmsg(db_()));
    throw std::runtime_error("error while preparing statement: " + msg);
  }
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

const char* sqlite_statement::column_text(int i) const
{
  return (const char*)sqlite3_column_text(stmt_, i);
}

int sqlite_statement::column_int(int i) const
{
  return sqlite3_column_int(stmt_, i);
}

double sqlite_statement::column_double(int i) const
{
  return sqlite3_column_double(stmt_, i);
}

int sqlite_statement::column_type(int i) const
{
  return sqlite3_column_type(stmt_, i);
}

int sqlite_statement::bind(int i, double value)
{
  return sqlite3_bind_double(stmt_, i, value);
}

int sqlite_statement::bind(int i, int value)
{
  return sqlite3_bind_int(stmt_, i, value);
}

int sqlite_statement::bind(int i, long value)
{
  return sqlite3_bind_int(stmt_, i, value);
}

int sqlite_statement::bind(int i, unsigned int value)
{
  return sqlite3_bind_int(stmt_, i, value);
}

int sqlite_statement::bind(int i, const char *value)
{
  return sqlite3_bind_text(stmt_, i, value, -1, 0);
}

int sqlite_statement::bind_null(int i)
{
  return sqlite3_bind_null(stmt_, i);
}

}

}
