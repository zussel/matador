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
#include "mysql_database.hpp"
#include "mysql_exception.hpp"
#include "mysql_prepared_result.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <cstring>
#include <sstream>
#include <tools/date.hpp>
#include <tools/time.hpp>

namespace oos {

namespace mysql {

mysql_statement::mysql_statement(mysql_database &db)
  : db_(db)
  , result_size(0)
  , host_size(0)
  , stmt(mysql_stmt_init(db()))
  , host_array(0)
{
}

mysql_statement::mysql_statement(mysql_database &db, const sql &s)
  : db_(db)
  , result_size(0)
  , host_size(0)
  , stmt(mysql_stmt_init(db()))
  , host_array(0)
{
  prepare(s);
}

mysql_statement::~mysql_statement()
{
  clear();
  mysql_stmt_close(stmt);
}

void mysql_statement::prepare(const sql &s)
{
  reset();
  
  str(s.prepare());
  // parse sql to create result and host arrays
  result_size = s.result_size();
  host_size = s.host_size();
  if (host_size) {
    host_array = new MYSQL_BIND[host_size];
    memset(host_array, 0, host_size * sizeof(MYSQL_BIND));
    length_vector.assign(host_size, 0);
  }
  
  int res = mysql_stmt_prepare(stmt, str().c_str(), str().size());
  if (res > 0) {
    throw_stmt_error(res, stmt, "mysql", str());
  }
}

void mysql_statement::reset()
{
  mysql_stmt_reset(stmt);
}

void mysql_statement::clear()
{
  for (int i = 0; i < host_size; ++i) {
    if (host_array[i].buffer) {
      delete [] static_cast<char*>(host_array[i].buffer);
    }
  }
  result_size = 0;
  host_size = 0;
  mysql_stmt_free_result(stmt);
  delete [] host_array;
}

result* mysql_statement::execute()
{
  if (host_array) {
    int res = mysql_stmt_bind_param(stmt, host_array);
    if (res > 0) {
      throw_stmt_error(res, stmt, "mysql", str());
    }
  }
  int res = mysql_stmt_execute(stmt);
  if (res > 0) {
    throw_stmt_error(res, stmt, "mysql", str());
  }
  res = mysql_stmt_store_result(stmt);
  if (res > 0) {
    throw_stmt_error(res, stmt, "mysql", str());
  }
  return new mysql_prepared_result(stmt, result_size);
}

void mysql_statement::write(const char *, char x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, short x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_SHORT, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, int x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONG, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, long x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONGLONG, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, unsigned char x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, unsigned short x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_SHORT, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, unsigned int x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONG, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, unsigned long x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONGLONG, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, bool x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TINY, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, float x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_FLOAT, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, double x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_DOUBLE, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const char *x, int s)
{
  bind_value(host_array[host_index], MYSQL_TYPE_VAR_STRING, x, s, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const std::string &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_STRING, x.data(), x.size(), host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const oos::date &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_DATE, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const oos::time &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_TIMESTAMP, x, host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const varchar_base &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_VAR_STRING, x.c_str(), x.size(), host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const object_base_ptr &x)
{
  bind_value(host_array[host_index], MYSQL_TYPE_LONG, x.id(), host_index);
  ++host_index;
}

void mysql_statement::write(const char *, const object_container &)
{
}

void mysql_statement::write(const char *id, const primary_key_base &x)
{
  x.serialize(id, *this);
}


database& mysql_statement::db()
{
  return db_;
}

const database& mysql_statement::db() const
{
  return db_;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::date &x, int /*index*/)
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

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::time &x, int /*index*/)
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
  mt->hour = (unsigned int)x.hour();
  mt->minute = (unsigned int)x.minute();
  mt->second = (unsigned int)x.second();
  mt->second_part = (unsigned long)x.milli_second() * 1000;
  mt->time_type  = MYSQL_TIMESTAMP_DATETIME;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, int index)
{
  bind.buffer = 0;
  bind.buffer_length = 0;
  bind.length = &length_vector.at(index);
  bind.buffer_type = type;
  bind.is_null = 0;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const char *value, int size, int /*index*/)
{
  if (bind.buffer == 0) {
    // allocating memory
    bind.buffer = new char[size];
    bind.buffer_length = size;
  }
#ifdef WIN32
    strncpy_s(static_cast<char*>(bind.buffer), size, value, _TRUNCATE);
#else
    strncpy(static_cast<char*>(bind.buffer), value, size);
#endif
  bind.buffer_type = type;
  bind.is_null = 0;
}

void mysql_statement::bind_value(MYSQL_BIND &bind, enum_field_types type, const object_base_ptr &value, int index)
{
  bind_value(bind, type, value.id(), index);
}

}

}
