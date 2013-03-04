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

#include "object/object_atomizable.hpp"
#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

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
  : db_(db)
  , stmt_(0)
{
}

sqlite_statement::~sqlite_statement()
{
  finalize();
}

void sqlite_statement::prepare(const sql &s)
{
  reset();
  
  sqlstr = s.prepare();

  // destroy statement
  finalize();
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_(), sqlstr.c_str(), sqlstr.size(), &stmt_, 0);
  throw_error(ret, db_(), "sqlite3_prepare_v2", sqlstr);
}

void sqlite_statement::reset()
{
  sqlite3_reset(stmt_);
  sqlite3_clear_bindings(stmt_);
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

void sqlite_statement::bind(object_atomizable *o)
{
  reset();
  host_index = 0;
  o->serialize(*this);
}

/*
int sqlite_statement::column_count() const
{
  return sqlite3_column_count(stmt_);
}

const char* sqlite_statement::column_name(int i) const
{
  return sqlite3_column_name(stmt_, i);
}
*/
void sqlite_statement::write(const char*, bool x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, char x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, short x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, int x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, long x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, unsigned char x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, unsigned short x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, unsigned int x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, unsigned long x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char*, float x)
{
  int ret = sqlite3_bind_double(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_double");
}

void sqlite_statement::write(const char*, double x)
{
  int ret = sqlite3_bind_double(stmt_, ++host_index, x);
  throw_error(ret, db_(), "sqlite3_bind_double");
}

void sqlite_statement::write(const char*, const char *x, int len)
{
  int ret = sqlite3_bind_text(stmt_, ++host_index, x, len, 0);
  throw_error(ret, db_(), "sqlite3_bind_text");
}

void sqlite_statement::write(const char*, const std::string &x)
{
  int ret = sqlite3_bind_text(stmt_, ++host_index, x.c_str(), x.size(), 0);
  throw_error(ret, db_(), "sqlite3_bind_text");
}

void sqlite_statement::write(const char*, const varchar_base &x)
{
  int ret = sqlite3_bind_text(stmt_, ++host_index, x.c_str(), x.size(), 0);
  throw_error(ret, db_(), "sqlite3_bind_text");
}

void sqlite_statement::write(const char *id, const object_base_ptr &x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x.id());
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char *id, const object_container &x)
{}

/*
int sqlite_statement::column_type(int i) const
{
  return sqlite3_column_type(stmt_, i);
}
*/

}

}
