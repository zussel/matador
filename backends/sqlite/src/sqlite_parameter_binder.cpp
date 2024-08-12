#include "sqlite_parameter_binder.h"
#include "sqlite_error.hpp"

#include "matador/utils/string.hpp"

#include <cstring>

namespace matador::backends::sqlite {

sqlite_parameter_binder::sqlite_parameter_binder(sqlite3 *db, sqlite3_stmt *stmt)
: db_(db)
, stmt_(stmt)
{}

void sqlite_parameter_binder::bind(size_t pos, char i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, short i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, int i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, long i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, long long int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, unsigned char i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, unsigned short i)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, unsigned int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, unsigned long i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, unsigned long long int i)
{
  int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), i);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, bool b)
{
  int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), b);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, float d)
{
  int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), d);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, double d)
{
  int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), d);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(size_t pos, const char *str)
{
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), str, static_cast<int>(strlen(str)), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(size_t pos, const char *x, size_t size)
{
  auto len = strlen(x);
  size = (len > size) ? size : len;
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), x, static_cast<int>(size), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(size_t pos, const std::string &str)
{
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), str.c_str(), static_cast<int>(str.size()), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(size_t pos, const std::string &x, size_t size)
{
  auto len = x.size();
  if (size == 0) {
    size = len;
  } else {
    size = (len > size) ? size : len;
  }
  int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), x.data(), static_cast<int>(size), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(size_t pos, const utils::blob &blob)
{

}

void sqlite_parameter_binder::bind(size_t pos, const time &time)
{
  auto time_string = std::make_shared<std::string>(matador::utils::to_string(time, "%Y-%m-%dT%T.%f"));
  bind(pos, *time_string);
  host_strings_.push_back(time_string);
}

void sqlite_parameter_binder::bind(size_t pos, const date &date)
{
  auto date_string = std::make_shared<std::string>(matador::utils::to_string(date, utils::date_format::ISO8601));
  bind(pos, *date_string);
  host_strings_.push_back(date_string);
}

}