#include "matador/db/mssql/mssql_result.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
//#include "matador/utils/basic_identifier.hpp"

#include <cstring>

namespace matador {

namespace mssql {

mssql_result::mssql_result(SQLHANDLE stmt)
  : affected_rows_(0)
  , rows(0)
  , fields_(0)
  , stmt_(stmt)
{
  // get row and column information
  SQLLEN r(0);
  SQLRETURN ret = SQLRowCount(stmt, &r);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");

  if (r != SQL_ERROR && r >= 0) {
    rows = (unsigned long) r;
  }

  SQLSMALLINT columns = 0;
  ret = SQLNumResultCols(stmt, &columns);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
}

mssql_result::~mssql_result()
{
  SQLCloseCursor(stmt_);
  SQLFreeStmt(stmt_, SQL_CLOSE);
}

const char* mssql_result::column(size_type) const
{
  return nullptr;
}

bool mssql_result::fetch()
{
//  SQLRETURN ret = SQLSetStmtAttr(stmt_, SQL_ROWSET_SIZE, (void *) 2, SQL_NTS);
//  throw_error(ret, SQL_HANDLE_DBC, stmt_, "mssql", "error on creating sql statement");
//  ret = SQLSetStmtAttr(stmt_, SQL_ATTR_CURSOR_TYPE, (void *) SQL_CURSOR_KEYSET_DRIVEN, SQL_NTS);
//  throw_error(ret, SQL_HANDLE_DBC, stmt_, "mssql", "error on creating sql statement");
//  ret = SQLSetStmtAttr(stmt_, SQL_ATTR_CONCURRENCY, (void *) SQL_CONCUR_LOCK, SQL_NTS);
//  throw_error(ret, SQL_HANDLE_DBC, stmt_, "mssql", "error on creating sql statement");

  SQLRETURN ret = SQLFetch(stmt_);
  if (SQL_SUCCEEDED(ret)) {
    return true;
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
    return false;
  }
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

void mssql_result::read_value(const char *id, int index, int row, char &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, short &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, int &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, long &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, long long &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, unsigned char &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, unsigned short &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, unsigned int &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, unsigned long &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, unsigned long long &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, bool &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, float &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, double &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *, int index, int row, char *x, long size)
{
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, index, SQL_C_CHAR, x, size, &info);
  if (ret == SQL_SUCCESS) {
    return;
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void mssql_result::read_value(const char *id, int index, int row, std::string &x, long size)
{
  read_column(id, index, x, size);
}

void mssql_result::read_value(const char *id, int index, int row, std::string &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, matador::date &x)
{
  read_column(id, index, x);
}

void mssql_result::read_value(const char *id, int index, int row, matador::time &x)
{
  read_column(id, index, x);
}

void mssql_result::read_column(const char *, int index, std::string &val)
{
  char buf[1024];
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, index, SQL_C_CHAR, buf, 1024, &info);
  if (SQL_SUCCEEDED(ret)) {
    val.assign(buf, info);
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void mssql_result::read_column(const char *, int index, std::string &val, size_t s)
{
  std::vector<char> buf(s, 0);
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, index, SQL_C_CHAR, buf.data(), s, &info);
  if (SQL_SUCCEEDED(ret)) {
    val.assign(buf.data(), info);
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }

}

void mssql_result::read_column(const char *, int index, char &val)
{
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), SQL_C_CHAR, &val, 0, &info);
  if (SQL_SUCCEEDED(ret)) {
    return;
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void mssql_result::read_column(const char *, int index, unsigned char &val)
{
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), SQL_C_CHAR, &val, 0, &info);
  if (SQL_SUCCEEDED(ret)) {
    return;
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void mssql_result::read_column(char const *, int index, date &x)
{
  SQL_DATE_STRUCT ds;

  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_TYPE_DATE, &ds, 0, &info);
  if (SQL_SUCCEEDED(ret)) {
    x.set(ds.day, ds.month, ds.year);
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void mssql_result::read_column(char const *, int index, time &x)
{
  SQL_TIMESTAMP_STRUCT ts;

  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_TYPE_TIMESTAMP, &ts, 0, &info);
  if (SQL_SUCCEEDED(ret)) {
    x.set(ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, ts.fraction / 1000 / 1000);
  } else {
    throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

bool mssql_result::prepare_fetch()
{
  if (!fetch()) {
    return false;
  }

  result_index_ = 0;
  return true;
}

bool mssql_result::finalize_fetch()
{
  return true;
}

}

}
