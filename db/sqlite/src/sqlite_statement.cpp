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

#include "sqlite_statement.hpp"
#include "sqlite_database.hpp"
#include "sqlite_exception.hpp"
#include "sqlite_prepared_result.hpp"

#include "database/row.hpp"

#include "tools/string.hpp"
#include "tools/varchar.hpp"
#include "tools/date.hpp"

#include <cstring>

#include <sqlite3.h>

namespace oos {

namespace sqlite {

void throw_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql = "")
{
  if (ec == SQLITE_OK) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << sqlite3_errmsg(db);
  if (!sql.empty()) {
    msg << " (" << sql << ")";
  }
  throw sqlite_exception(msg.str()); 
}

sqlite_statement::sqlite_statement(sqlite_database &db, const std::string stmt, std::shared_ptr<oos::object_base_producer> producer)
  : db_(db)
  , stmt_(0)
  , producer_(producer)
{
  str(stmt);
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_(), str().c_str(), str().size(), &stmt_, 0);
  throw_error(ret, db_(), "sqlite3_prepare_v2", str());

}

sqlite_statement::~sqlite_statement()
{
  clear();
}

detail::result_impl* sqlite_statement::execute()
{
  // get next row
  int ret = sqlite3_step(stmt_);

  return new sqlite_prepared_result(stmt_, ret, producer_);
}

void sqlite_statement::reset()
{
  if (stmt_) {
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
  }
}

void sqlite_statement::clear()
{
  if (!stmt_) {
    return;
  }
  int ret = sqlite3_finalize(stmt_);
  throw_error(ret, db_(), "sqlite3_finalize");
  stmt_ = 0;
  return;
}

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

void sqlite_statement::write(const char *, const oos::date &x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x.julian_date());
  throw_error(ret, db_(), "sqlite3_bind_int");}

void sqlite_statement::write(const char *id, const oos::time &x)
{
  // format time to ISO8601
  auto time_string = std::make_shared<std::string>(oos::to_string(x, "%F %T.%f"));
  write(id, *time_string);
  host_strings_.push_back(time_string);
}

void sqlite_statement::write(const char *, const object_base_ptr &x)
{
  int ret = sqlite3_bind_int(stmt_, ++host_index, x.id());
  throw_error(ret, db_(), "sqlite3_bind_int");
}

void sqlite_statement::write(const char *, const object_container &)
{}

void sqlite_statement::write(const char *id, const basic_identifier &x)
{
  x.serialize(id, *this);
}

}

}
