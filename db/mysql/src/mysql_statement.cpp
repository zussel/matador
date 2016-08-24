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

#include "mysql_statement.hpp"
#include "mysql_connection.hpp"
#include "mysql_exception.hpp"
#include "mysql_prepared_result.hpp"

#include "tools/string.hpp"
#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/identifiable_holder.hpp"

#include "sql/sql.hpp"

#include <cstring>

namespace oos {

namespace mysql {

mysql_statement::mysql_statement(mysql_connection &db, const oos::sql &stmt)
  : db_(db)
  , result_size(0)
  , host_size(0)
  , stmt_(mysql_stmt_init(db.handle()))
{
  str(db.dialect()->prepare(stmt));
  // Todo: use new sql compile style
  // parse sql to create result and host arrays
  result_size = db.dialect()->column_count();
  host_size = db.dialect()->bind_count();
  if (host_size) {
    host_array = new MYSQL_BIND[host_size];
    memset(host_array, 0, host_size * sizeof(MYSQL_BIND));
    length_vector.assign(host_size, 0);
  }

  int res = mysql_stmt_prepare(stmt_, str().c_str(), str().size());
  if (res > 0) {
    throw_stmt_error(res, stmt_, "mysql", str());
  }
}

mysql_statement::~mysql_statement()
{
  clear();
  mysql_stmt_close(stmt_);
}

void mysql_statement::reset()
{
  mysql_stmt_reset(stmt_);
}

void mysql_statement::clear()
{
  for (size_t i = 0; i < host_size; ++i) {
    if (host_array[i].buffer) {
      delete [] static_cast<char*>(host_array[i].buffer);
    }
  }
  delete [] host_array;
  host_array = nullptr;

  result_size = 0;
  host_size = 0;
  mysql_stmt_free_result(stmt_);
}

detail::result_impl* mysql_statement::execute()
{
  if (host_array) {
    int res = mysql_stmt_bind_param(stmt_, host_array);
    if (res > 0) {
      throw_stmt_error(res, stmt_, "mysql", str());
    }
  }
//  std::cout << str() << '\n';

  int res = mysql_stmt_execute(stmt_);
  if (res > 0) {
    throw_stmt_error(res, stmt_, "mysql", str());
  }
  res = mysql_stmt_store_result(stmt_);
  if (res > 0) {
    throw_stmt_error(res, stmt_, "mysql", str());
  }
  return new mysql_prepared_result(stmt_, (int)result_size);
}

void mysql_statement::serialize(const char *, char &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, short &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_SHORT, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, int &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONG, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, long &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONGLONG, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, unsigned char &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, unsigned short &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_SHORT, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, unsigned int &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONG, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, unsigned long &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONGLONG, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, bool &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, float &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_FLOAT, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, double &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_DOUBLE, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, char *x, size_t s)
{
  bind_value(host_array[host_index], MYSQL_TYPE_VAR_STRING, x, s, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, std::string &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_STRING, x.data(), x.size(), host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, oos::date &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_DATE, x, host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *, oos::time &x)
{
#if MYSQL_VERSION_ID < 50604
  // before mysql version 5.6.4 datetime
  // doesn't support fractional seconds
  // so we use a datetime string here
  std::string tstr = to_string(x, "%F %T.%f");
  bind_value(host_array[host_index], MYSQL_TYPE_VAR_STRING, tstr.c_str(), tstr.size(), host_index);
#else
  bind_value(host_array[host_index], MYSQL_TYPE_TIMESTAMP, x, host_index);
#endif
  ++host_index;
}

void mysql_statement::serialize(const char *, varchar_base &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_VAR_STRING, x.c_str(), x.size(), host_index);
  ++host_index;
}

void mysql_statement::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

void mysql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  }
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::date &x, size_t /*index*/)
{
  if (bind.buffer == 0) {
    size_t s = sizeof(MYSQL_TIME);
    bind.buffer = new char[s];
    bind.buffer_length = s;
  }
  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
  bind.buffer_type = type;
  bind.length = 0;
  bind.is_null = 0;
  MYSQL_TIME *mt = static_cast<MYSQL_TIME*>(bind.buffer);
  mt->day = (unsigned int)x.day();
  mt->month = (unsigned int)x.month();
  mt->year = (unsigned int)x.year();
  mt->time_type  = MYSQL_TIMESTAMP_DATE;
//  mt->hour = 0;
//  mt->minute = 0;
//  mt->second = 0;
//  mt->second_part = 0;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::time &x, size_t /*index*/)
{
  if (bind.buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    bind.buffer = new char[s];
    bind.buffer_length = s;
  }
  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
  bind.buffer_type = type;
  bind.length = 0;
  bind.is_null = 0;
  MYSQL_TIME *mt = static_cast<MYSQL_TIME*>(bind.buffer);
  mt->day = (unsigned int)x.day();
  mt->month = (unsigned int)x.month();
  mt->year = (unsigned int)x.year();
  mt->hour = (unsigned int)x.hour();
  mt->minute = (unsigned int)x.minute();
  mt->second = (unsigned int)x.second();
  mt->second_part = (unsigned long)x.milli_second() * 1000;
  mt->time_type  = MYSQL_TIMESTAMP_DATETIME;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, size_t index)
{
  bind.buffer = nullptr;
  bind.buffer_length = 0;
  bind.length = &length_vector.at(index);
  bind.buffer_type = type;
  bind.is_null = 0;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const char *value, size_t size, size_t /*index*/)
{
  if (bind.buffer_length < size) {
    // reallocate memory
    delete [] static_cast<char*>(bind.buffer);
    bind.buffer = nullptr;
    bind.buffer_length = 0;
  }
  if (bind.buffer == nullptr) {
    // allocating memory
    bind.buffer = new char[size];
  }
  bind.buffer_length = (unsigned long)size;
#ifdef WIN32
  strncpy_s(static_cast<char*>(bind.buffer), size, value, _TRUNCATE);
#else
  strncpy(static_cast<char*>(bind.buffer), value, size);
#endif
  bind.buffer_type = type;
  bind.is_null = 0;
}

}

}
