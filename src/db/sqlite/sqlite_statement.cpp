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

#include "matador/db/sqlite/sqlite_statement.hpp"
#include "matador/db/sqlite/sqlite_connection.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"
#include "matador/db/sqlite/sqlite_prepared_result.hpp"

#include "matador/sql/row.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/varchar.hpp"
#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/basic_identifier.hpp"

#include <cstring>

#include <sqlite3.h>

namespace matador {

namespace sqlite {

sqlite_statement::sqlite_statement(sqlite_connection &db, const std::string &stmt)
  : db_(db)
  , stmt_(nullptr)
{
  str(stmt);
  // prepare sqlite statement
  int ret = sqlite3_prepare_v2(db_.handle(), str().c_str(), str().size(), &stmt_, nullptr);
  throw_error(ret, db_.handle(), "sqlite3_prepare_v2", str());
}

sqlite_statement::~sqlite_statement()
{
  clear();
}

detail::result_impl* sqlite_statement::execute()
{
  // get next row
  int ret = sqlite3_step(stmt_);

  if (ret != SQLITE_ROW && ret != SQLITE_DONE) {
    throw_error(ret, db_.handle(), "sqlite3_step");
  }
  return new sqlite_prepared_result(stmt_, ret);
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
  throw_error(ret, db_.handle(), "sqlite3_finalize");
  stmt_ = nullptr;
}

void sqlite_statement::serialize(const char*, bool &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, char &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, short &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, int &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, long &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, unsigned char &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, unsigned short &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, unsigned int &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, unsigned long &x)
{
  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char*, float &x)
{
  int ret = sqlite3_bind_double(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_double");
}

void sqlite_statement::serialize(const char*, double &x)
{
  int ret = sqlite3_bind_double(stmt_, (int)++host_index, x);
  throw_error(ret, db_.handle(), "sqlite3_bind_double");
}

void sqlite_statement::serialize(const char*, char *x, size_t len)
{
  int ret = sqlite3_bind_text(stmt_, (int)++host_index, x, (int)len, nullptr);
  throw_error(ret, db_.handle(), "sqlite3_bind_text");
}

void sqlite_statement::serialize(const char*, std::string &x)
{
  int ret = sqlite3_bind_text(stmt_, (int)++host_index, x.c_str(), (int)x.size(), nullptr);
  throw_error(ret, db_.handle(), "sqlite3_bind_text");
}

void sqlite_statement::serialize(const char*, varchar_base &x)
{
  int ret = sqlite3_bind_text(stmt_, (int)++host_index, x.c_str(), (int)x.size(), nullptr);
  throw_error(ret, db_.handle(), "sqlite3_bind_text");
}

void sqlite_statement::serialize(const char *id, matador::date &x)
{
  auto date_string = std::make_shared<std::string>(matador::to_string(x, date_format::ISO8601));
  serialize(id, *date_string);
  host_strings_.push_back(date_string);
//  int ret = sqlite3_bind_int(stmt_, (int)++host_index, x.julian_date());
//  throw_error(ret, db_.handle(), "sqlite3_bind_int");
}

void sqlite_statement::serialize(const char *id, matador::time &x)
{
  // format time to ISO8601
  auto time_string = std::make_shared<std::string>(matador::to_string(x, "%Y-%m-%dT%T.%f"));
  serialize(id, *time_string);
  host_strings_.push_back(time_string);
}

void sqlite_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    sqlite3_bind_null(stmt_, (int)++host_index);
  }
}

void sqlite_statement::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

}

}
