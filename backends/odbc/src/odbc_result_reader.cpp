#include "odbc_result_reader.hpp"

#include <sqlext.h>

namespace matador::backends::odbc {

odbc_result_reader::odbc_result_reader(SQLHANDLE stmt)
: stmt_(stmt)
{
  // get row and column information
  SQLLEN r(0);
  SQLRETURN ret = SQLRowCount(stmt, &r);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");

  if (r != SQL_ERROR && r >= 0) {
    row_count_ = static_cast<size_t>(r);
  }

  SQLSMALLINT columns = 0;
  ret = SQLNumResultCols(stmt, &columns);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "mssql");

  column_count_ = static_cast<size_t>(columns);
}

odbc_result_reader::~odbc_result_reader()
{
  SQLFreeStmt(stmt_, SQL_CLOSE);
}

size_t odbc_result_reader::column_count() const
{
  return column_count_;
}

const char* odbc_result_reader::column(size_t index) const
{
  return nullptr;
}

bool odbc_result_reader::fetch()
{
  if (const SQLRETURN ret = SQLFetch(stmt_); SQL_SUCCEEDED(ret)) {
    return true;
  } else {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "odbc");
    return false;
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, char &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, short &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, int &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, long &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, long long int &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, unsigned char &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, unsigned short &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, unsigned int &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, unsigned long &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, unsigned long long int &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, bool &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, float &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, double &value)
{
  read_column(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, char *value, size_t s)
{
  SQLLEN info = 0;
  if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_CHAR, value, s, &info); ret != SQL_SUCCESS) {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, std::string &value)
{
  char buf[1024];
  SQLLEN info = 0;
  SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_CHAR, buf, 1024, &info);
  if (SQL_SUCCEEDED(ret)) {
    if (info > 0) {
      value.assign(buf, info);
    } else {
      value.clear();
    }
  } else {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, std::string &value, size_t s)
{
  if (s == 0) {
    s = 8000;
  }
  std::vector<char> buf(s, 0);
  SQLLEN info = 0;
  if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_CHAR, buf.data(), static_cast<SQLLEN>(s), &info); SQL_SUCCEEDED(ret)) {
    if (info > 0) {
      value.assign(buf.data(), info);
    } else {
      value.clear();
    }
  } else {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, time &value) {
  SQL_TIMESTAMP_STRUCT ts;

  SQLLEN info = 0;
  if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_TYPE_TIMESTAMP, &ts, 0, &info); SQL_SUCCEEDED(ret)) {
    if (info != SQL_NULL_DATA) {
      value.set(ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, ts.fraction / 1000 / 1000);
    }
  } else {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, date &value) {
  SQL_DATE_STRUCT ds;

  SQLLEN info = 0;
  if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), SQL_C_TYPE_DATE, &ds, 0, &info); SQL_SUCCEEDED(ret)) {
    if (info != SQL_NULL_DATA) {
      value.set(ds.day, ds.month, ds.year);
    }
  } else {
    throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
  }
}

void odbc_result_reader::read_value(const char *id, size_t index, utils::blob &value) {
  query_result_reader::read_value(id, index, value);
}

void odbc_result_reader::read_value(const char *id, size_t index, sql::value &val, size_t size)
{
  query_result_reader::read_value(id, index, val, size);
}

int odbc_result_reader::type2int(data_type type) {
  switch(type) {
    case data_type::type_char:
    case data_type::type_short:
      return SQL_C_SSHORT;
    case data_type::type_int:
      return SQL_C_SLONG;
    case data_type::type_long:
#ifdef __LP64__
      return SQL_C_SBIGINT;
#else
      return SQL_C_SLONG;
#endif
    case data_type::type_long_long:
      return SQL_C_SBIGINT;
    case data_type::type_unsigned_char:
      return SQL_C_SHORT;
    case data_type::type_unsigned_short:
      return SQL_C_USHORT;
    case data_type::type_unsigned_int:
      return SQL_C_ULONG;
    case data_type::type_unsigned_long:
#ifdef __LP64__
      return SQL_C_UBIGINT;
#else
      return SQL_C_ULONG;
#endif
    case data_type::type_unsigned_long_long:
      return SQL_C_UBIGINT;
    case data_type::type_bool:
      return SQL_C_BIT;
    case data_type::type_float:
      return SQL_C_FLOAT;
    case data_type::type_double:
      return SQL_C_DOUBLE;
    case data_type::type_char_pointer:
    case data_type::type_varchar:
    case data_type::type_text:
      return SQL_C_CHAR;
    case data_type::type_date:
      return SQL_C_TYPE_DATE;
    case data_type::type_time:
      return SQL_C_TYPE_TIMESTAMP;
    default:
    {
      throw std::logic_error("mssql statement: unknown type");
    }
  }
}

}
