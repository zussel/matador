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

#include "database/mssql/mssql_statement.hpp"
#include "database/mssql/mssql_database.hpp"
#include "database/mssql/mssql_exception.hpp"
#include "database/mssql/mssql_prepared_result.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <sstream>

namespace oos {

namespace mssql {

mssql_statement::mssql_statement(mssql_database &db)
  : db_(db)
  , result_size(0)
  , host_size(0)
{
//  std::cout << "creating mssql statement " << this << "\n";
}

mssql_statement::mssql_statement(mssql_database &db, const sql &s)
  : db_(db)
  , result_size(0)
  , host_size(0)
{
//  std::cout << "creating mssql statement " << this << "\n";
  prepare(s);
}
mssql_statement::~mssql_statement()
{
//  std::cout << "destroying mssql statement " << this << "\n";
  clear();
}

void mssql_statement::prepare(const sql &s)
{
  reset();
  
  str(s.prepare());
  // parse sql to create result and host arrays
  result_size = s.result_size();
//  if (result_size) {
  if (s.result_size()) {
    
    sql::const_iterator first = s.result_begin();
    sql::const_iterator last = s.result_end();
    while (first != last) {
      prepare_result_column(*first++);
    }
  }
  host_size = s.host_size();
}

void mssql_statement::reset()
{
}

void mssql_statement::clear()
{
  result_size = 0;
  host_size = 0;
  host_data.clear();
}

result* mssql_statement::execute()
{
  return 0;
}

void mssql_statement::write(const char *, char x)
{
  ++host_index;
}

void mssql_statement::write(const char *, short x)
{
  ++host_index;
}

void mssql_statement::write(const char *, int x)
{
  ++host_index;
}

void mssql_statement::write(const char *, long x)
{
  ++host_index;
}

void mssql_statement::write(const char *, unsigned char x)
{
  ++host_index;
}

void mssql_statement::write(const char *, unsigned short x)
{
  ++host_index;
}

void mssql_statement::write(const char *, unsigned int x)
{
  ++host_index;
}

void mssql_statement::write(const char *, unsigned long x)
{
  ++host_index;
}

void mssql_statement::write(const char *, bool x)
{
  ++host_index;
}

void mssql_statement::write(const char *, float x)
{
  ++host_index;
}

void mssql_statement::write(const char *, double x)
{
  ++host_index;
}

void mssql_statement::write(const char *, const char *x, int s)
{
  ++host_index;
}

void mssql_statement::write(const char *, const std::string &x)
{
  ++host_index;
}

void mssql_statement::write(const char *, const varchar_base &x)
{
  ++host_index;
}

void mssql_statement::write(const char *, const object_base_ptr &x)
{
  ++host_index;
}

void mssql_statement::write(const char *, const object_container &)
{
}

void mssql_statement::prepare_result_column(const sql::field_ptr &fptr)
{
}

database& mssql_statement::db()
{
  return db_;
}

const database& mssql_statement::db() const
{
  return db_;
}

}

}
