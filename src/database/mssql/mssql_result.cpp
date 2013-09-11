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

#include "database/mssql/mssql_result.hpp"
#include "database/row.hpp"

#include "database/mssql/mssql_exception.hpp"

namespace oos {

namespace mssql {

mssql_result::mssql_result(SQLHANDLE stmt)
  : affected_rows_(0)
  , rows(0)
  , fields_(0)
  , stmt_(stmt)
{
  // get row and column information
  SQLRETURN ret = SQLRowCount(stmt, (SQLLEN*)&rows);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "couldn't retrieve row count");

  SQLSMALLINT columns = 0;
  ret = SQLNumResultCols(stmt, &columns);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "couldn't get column count");
}

mssql_result::~mssql_result()
{
}

const char* mssql_result::column(size_type /*c*/) const
{
  return 0;
}

bool mssql_result::fetch()
{
  SQLRETURN ret = SQLFetch(stmt_);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on fetching next row");
  return rows-- > true;
}

mssql_result::size_type mssql_result::affected_rows() const
{
  return affected_rows_;
}

mssql_result::size_type mssql_result::result_rows() const
{
  return rows;
}

mssql_result::size_type mssql_result::fields() const
{
  return fields_;
}

void mssql_result::read(const char *id, char &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, short &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, int &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, long &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, unsigned char &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, unsigned short &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, unsigned int &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, unsigned long &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, bool &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, float &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, double &x)
{
  read_column(id, x);
}

void mssql_result::read(const char */*id*/, char */*x*/, int /*s*/)
{
}

void mssql_result::read(const char *id, varchar_base &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, std::string &x)
{
  read_column(id, x);
}

void mssql_result::read(const char */*id*/, object_base_ptr &/*x*/)
{
}

void mssql_result::read(const char */*id*/, object_container &/*x*/)
{
}


}

}
