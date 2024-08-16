#include "odbc_parameter_binder.hpp"
#include "odbc_error.hpp"

#include <sql.h>
#include <sqltypes.h>

#include <cstring>

namespace matador::backends::odbc {

odbc_parameter_binder::odbc_parameter_binder(SQLHANDLE stmt)
: stmt_(stmt)
{}

void odbc_parameter_binder::bind(size_t pos, char i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, short i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, int i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, long i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, long long int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, unsigned char i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, unsigned short i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, unsigned int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, unsigned long i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, unsigned long long int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, bool b)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), b);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, float d)
{
  int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), d);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, double d)
{
  int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), d);
  throw_odbc_error(ret, db_, "sqlite3_bind_int");
}

void odbc_parameter_binder::bind(size_t pos, const char *str)
{
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), str, static_cast<int>(strlen(str)), nullptr);
  throw_odbc_error(ret, db_, "sqlite3_bind_text");
}

void odbc_parameter_binder::bind(size_t pos, const char *x, size_t size)
{
  auto len = strlen(x);
  size = (len > size) ? size : len;
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), x, static_cast<int>(size), nullptr);
  throw_odbc_error(ret, db_, "sqlite3_bind_text");
}

void odbc_parameter_binder::bind(size_t pos, const std::string &str)
{
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), str.c_str(), static_cast<int>(str.size()), nullptr);
  throw_odbc_error(ret, db_, "sqlite3_bind_text");
}

void odbc_parameter_binder::bind(size_t pos, const std::string &x, size_t size)
{
  auto len = x.size();
  if (size == 0) {
    size = len;
  } else {
    size = (len > size) ? size : len;
  }
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), x.data(), static_cast<int>(size), nullptr);
  throw_odbc_error(ret, db_, "sqlite3_bind_text");
}

void odbc_parameter_binder::bind(size_t pos, const utils::blob &blob)
{

}

void odbc_parameter_binder::bind(size_t pos, const time &time)
{

}

void odbc_parameter_binder::bind(size_t pos, const date &date)
{

}

}