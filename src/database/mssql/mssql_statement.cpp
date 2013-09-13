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
  std::cout << "creating mssql statement " << this << "\n";
  if (!db()) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db(), &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db(), "mssql", "error on creating sql statement");
}

mssql_statement::mssql_statement(mssql_database &db, const sql &s)
  : db_(db)
  , result_size(0)
  , host_size(0)
{
  std::cout << "creating mssql statement " << this << "\n";
  if (!db()) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db(), &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db(), "mssql", "error on creating sql statement");
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
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, short x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, int x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, long x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned char x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned short x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned int x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned long x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, bool x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, float x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, double x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, const char *x, int s)
{
  bind_value(x, s, ++host_index);
}

void mssql_statement::write(const char *, const std::string &x)
{
  bind_value(x.data(), x.size(), ++host_index);
}

void mssql_statement::write(const char *, const varchar_base &x)
{
  bind_value(x.c_str(), x.size(), ++host_index);
}

void mssql_statement::write(const char *, const object_base_ptr &x)
{
  bind_value(x.id(), ++host_index);
}

void mssql_statement::write(const char *, const object_container &)
{
}

void mssql_statement::bind_value(const char *val, int size, int index)
{
  SQLLEN len = 0;
  SQLRETURN ret = SQLBindParameter(stmt_, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, size, 0, (SQLPOINTER)val, 0, &len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::prepare_result_column(const sql::field_ptr &/*fptr*/)
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

long mssql_statement::type2int(data_type_t type)
{
  switch(type) {
    case type_char:
      return SQL_C_STINYINT;
    case type_short:
      return SQL_C_SSHORT;
    case type_int:
      return SQL_C_SLONG;
    case type_long:
      return SQL_C_SLONG;
    case type_unsigned_char:
      return SQL_C_UTINYINT;
    case type_unsigned_short:
      return SQL_C_USHORT;
    case type_unsigned_int:
      return SQL_C_ULONG;
    case type_unsigned_long:
      return SQL_C_ULONG;
    case type_bool:
      return SQL_C_STINYINT;
    case type_float:
      return SQL_C_FLOAT;
    case type_double:
      return SQL_C_DOUBLE;
    case type_char_pointer:
      return SQL_C_CHAR;
    case type_varchar:
      return SQL_C_CHAR;
    case type_text:
      return SQL_C_CHAR;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

long mssql_statement::type2sql(data_type_t type)
{
  switch(type) {
    case type_char:
      return SQL_SMALLINT;
    case type_short:
      return SQL_SMALLINT;
    case type_int:
      return SQL_INTEGER;
    case type_long:
      return SQL_INTEGER;
    case type_unsigned_char:
      return SQL_SMALLINT;
    case type_unsigned_short:
      return SQL_SMALLINT;
    case type_unsigned_int:
      return SQL_INTEGER;
    case type_unsigned_long:
      return SQL_INTEGER;
    case type_bool:
      return SQL_SMALLINT;
    case type_float:
      return SQL_FLOAT;
    case type_double:
      return SQL_DOUBLE;
    case type_char_pointer:
      return SQL_VARCHAR;
    case type_varchar:
      return SQL_VARCHAR;
    case type_text:
      return SQL_VARCHAR;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

}

}
