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
#include "database/mysql/mysql_prepared_result.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <sstream>

namespace oos {

namespace mysql {

mysql_statement::mysql_statement(mysql_database &db)
  : db_(db)
  , result_size(0)
  , host_size(0)
  , stmt(mysql_stmt_init(db()))
  , host_array(0)
{
//  std::cout << "creating mysql statement " << this << "\n";
}

mysql_statement::mysql_statement(mysql_database &db, const sql &s)
  : db_(db)
  , result_size(0)
  , host_size(0)
  , stmt(mysql_stmt_init(db()))
  , host_array(0)
{
//  std::cout << "creating mysql statement " << this << "\n";
  prepare(s);
}
mysql_statement::~mysql_statement()
{
//  std::cout << "destroying mysql statement " << this << "\n";
  clear();
  mysql_stmt_close(stmt);
}

void mysql_statement::prepare(const sql &s)
{
  reset();
  
  str(s.prepare());
  // parse sql to create result and host arrays
  result_size = s.result_size();
//  if (result_size) {
//  if (s.result_size()) {
    /*
     * prepare result array for mysql:
     *
     * allocate MYSQL_BIND array and
     * setup each item with the correct type
     * and allocate enough memory for the value
     */
    /*
    result_array = new MYSQL_BIND[s.result_size()];
    memset(result_array, 0, s.result_size() * sizeof(MYSQL_BIND));
    
    sql::const_iterator first = s.result_begin();
    sql::const_iterator last = s.result_end();
    while (first != last) {
      prepare_result_column(*first++);
    }
  }*/
  host_size = s.host_size();
  if (host_size) {
    host_array = new MYSQL_BIND[host_size];
    memset(host_array, 0, host_size * sizeof(MYSQL_BIND));
    length_vector.assign(host_size, 0);
  }
  
  if (mysql_stmt_prepare(stmt, str().c_str(), str().size())) {
    fprintf(stderr, " mysql_stmt_prepare(), SELECT failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
    exit(EXIT_FAILURE);
  }
}

void mysql_statement::reset()
{
  mysql_stmt_reset(stmt);
}

void mysql_statement::clear()
{
//  std::cout << "reset mysql_statement\n";
  for (int i = 0; i < host_size; ++i) {
//    std::cout << "\tfreeing host_array[" << i << "].buffer: " << host_array[i].buffer << "\n";
    if (host_array[i].buffer) {
      delete [] static_cast<char*>(host_array[i].buffer);
    }
  }
  /*
  for (int i = 0; i < result_size; ++i) {
    if (result_array[i].buffer) {
      delete [] static_cast<char*>(result_array[i].buffer);
    }
  }
  */
  result_size = 0;
  host_size = 0;
  mysql_stmt_free_result(stmt);
//  delete [] result_array;
  delete [] host_array;
}

result* mysql_statement::execute()
{
//  std::cout << "Executing prepared statement: " << str() << "\n";
//  if (result_array) {
//    std::cout << "\tresult_array: " << result_array << "\n";
/*    if (mysql_stmt_bind_result(stmt, result_array)) {
      fprintf(stderr, " mysql_stmt_bind_result() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(EXIT_FAILURE);
    }
  }*/
  if (host_array) {
//    std::cout << "\thost_array: " << host_array << "\n";
    if (mysql_stmt_bind_param(stmt, host_array)) {
      fprintf(stderr, " mysql_stmt_bind_param() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(EXIT_FAILURE);
    }
  }
  if (mysql_stmt_execute(stmt)) {
    fprintf(stderr, " mysql_stmt_execute(), failed\n");
    fprintf(stderr, " %d: %s\n", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
    exit(EXIT_FAILURE);
  }
  if (mysql_stmt_store_result(stmt)) {
    fprintf(stderr, " mysql_stmt_store_result(), failed\n");
    fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
    exit(EXIT_FAILURE);
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

void mysql_statement::write(const char *, const std::string &/*x*/)
{
//  bind_value(host_array[host_index], MYSQL_TYPE_STRING, x.data(), x.size(), host_index);
  bind_value(host_array[host_index], MYSQL_TYPE_STRING, host_index);
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
/*
void mysql_statement::prepare_result_column(const sql::field_ptr &fptr)
{
  if (result_array[fptr->index].buffer == 0) {
    // allocating memory
//      std::cout << "allocating " << sizeof(T) << " bytes of memory\n";
    unsigned int size = sql::type_size(fptr->type);
    result_array[fptr->index].buffer = new char[size];
    memset(result_array[fptr->index].buffer, 0, size);
    result_array[fptr->index].buffer_length = size;
  }
  result_array[fptr->index].buffer_type = mysql_statement::type_enum(fptr->type);
  result_array[fptr->index].is_null = 0;
}
*/
enum_field_types mysql_statement::type_enum(data_type_t type)
{
  switch(type) {
    case type_char:
      return MYSQL_TYPE_TINY;
    case type_short:
      return MYSQL_TYPE_SHORT;
    case type_int:
      return MYSQL_TYPE_LONG;
    case type_long:
      return MYSQL_TYPE_LONGLONG;
    case type_unsigned_char:
      return MYSQL_TYPE_TINY;
    case type_unsigned_short:
      return MYSQL_TYPE_SHORT;
    case type_unsigned_int:
      return MYSQL_TYPE_LONG;
    case type_unsigned_long:
      return MYSQL_TYPE_LONGLONG;
    case type_bool:
      return MYSQL_TYPE_TINY;
    case type_float:
      return MYSQL_TYPE_FLOAT;
    case type_double:
      return MYSQL_TYPE_DOUBLE;
    case type_char_pointer:
      return MYSQL_TYPE_VAR_STRING;
    case type_varchar:
      return MYSQL_TYPE_VAR_STRING;
    case type_text:
      return MYSQL_TYPE_STRING;
    default:
      {
        throw std::logic_error("mysql statement: unknown type");
      }
    }
}

database& mysql_statement::db()
{
  return db_;
}

const database& mysql_statement::db() const
{
  return db_;
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
//      std::cout << "allocating " << sizeof(T) << " bytes of memory\n";
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
