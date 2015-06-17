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

#include "mssql_result.hpp"
#include "mssql_exception.hpp"

#include "database/row.hpp"

#include "object/object_ptr.hpp"
#include "object/object.hpp"

namespace oos {

namespace mssql {

mssql_result::mssql_result(SQLHANDLE stmt, bool free)
  : affected_rows_(0)
  , rows(0)
  , fields_(0)
  , free_(free)
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
  if (free_) {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
  }
}

const char* mssql_result::column(size_type /*c*/) const
{
  return 0;
}

bool mssql_result::fetch()
{
  SQLRETURN ret = SQLFetch(stmt_);
  if (SQL_SUCCEEDED(ret)) {
    return true;
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on fetching next row");
    return false;
  }
}

bool mssql_result::fetch(serializable *o)
{
  if (!fetch()) {
    return false;
  }
  
  get(o);

  return true;
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

int mssql_result::transform_index(int index) const
{
  return ++index;
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

void mssql_result::read(const char * /*id*/, char *x, int s)
{
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, result_index++, SQL_C_CHAR, x, s, &info);
  if (ret == SQL_SUCCESS) {
    return;
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}

void mssql_result::read(const char *id, varchar_base &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, std::string &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, oos::date &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, oos::time &x)
{
  read_column(id, x);
}

void mssql_result::read(const char *id, object_base_ptr &x)
{
  long val;
  read_column(id, val);
  x.id(val);
}

void mssql_result::read(const char * /*id*/, object_container &/*x*/)
{
}

void mssql_result::read(const char *id, primary_key_base &x) {
  x.deserialize(id, *this);
}

void mssql_result::read_column(const char *, unsigned long &val)
{
  char buf[NUMERIC_LEN];
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, result_index++, SQL_C_CHAR, buf, NUMERIC_LEN, &info);
  if (ret == SQL_SUCCESS) {    
    if (sscanf(buf, "%lu", &val) != 1) {
      throw_error("mssql", strerror(errno));
    }
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}

void mssql_result::read_column(const char *, std::string &val)
{
  char buf[1024];
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, result_index++, SQL_C_CHAR, buf, 1024, &info);
  if (ret == SQL_SUCCESS) {
    val.assign(buf, info);
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}

void mssql_result::read_column(const char *, varchar_base &val)
{
  char *buf = new char[val.capacity()];
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(result_index++), SQL_C_CHAR, buf, val.capacity(), &info);
  if (ret == SQL_SUCCESS) {
    val.assign(buf, static_cast<size_t>(info));
    delete [] buf;
  } else {
    delete [] buf;
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}


void mssql_result::read_column(char const *, date &x)
{
  SQL_DATE_STRUCT ds;

  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(result_index++), SQL_C_TYPE_DATE, &ds, 0, &info);
  if (ret == SQL_SUCCESS) {
    x.year(ds.year);
    x.month(ds.month);
    x.day(ds.day);
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}

void mssql_result::read_column(char const *, time &x)
{
  SQL_TIMESTAMP_STRUCT ts;

  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(result_index++), SQL_C_TYPE_TIMESTAMP, &ts, 0, &info);
  if (ret == SQL_SUCCESS) {
    x.set(ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, ts.fraction / 1000 / 1000);
  } else {
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
  }
}

}

}
