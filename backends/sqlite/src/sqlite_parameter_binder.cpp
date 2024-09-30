#include "sqlite_parameter_binder.h"
#include "sqlite_error.hpp"

#include "matador/utils/string.hpp"

#include <cstring>

namespace matador::backends::sqlite {

sqlite_parameter_binder::sqlite_parameter_binder(sqlite3 *db, sqlite3_stmt *stmt)
: db_(db)
, stmt_(stmt)
{}

void sqlite_parameter_binder::write_value(size_t pos, const char &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const short &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const int &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const long &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const long long int &x)
{
  const int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const unsigned char &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const unsigned short &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const unsigned int &x)
{
  const int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const unsigned long &x)
{
  const int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int64");
}

void sqlite_parameter_binder::write_value(size_t pos, const unsigned long long int &x)
{
  const int ret = sqlite3_bind_int64(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int64");
}

void sqlite_parameter_binder::write_value(size_t pos, const bool &x)
{
  const int ret = sqlite3_bind_int(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const float &x)
{
  const int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const double &x)
{
  const int ret = sqlite3_bind_double(stmt_, static_cast<int>(++pos), x);
  throw_sqlite_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::write_value(size_t pos, const char *x)
{
  host_strings_.emplace_back(x, strlen(x));
  const int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), host_strings_.back().c_str(), static_cast<int>(host_strings_.back().size()), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::write_value(size_t pos, const char *x, size_t size)
{
  const auto len = strlen(x);
  size = (len > size && size != 0) ? size : len;
  host_strings_.emplace_back(x, size);
  const int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), host_strings_.back().c_str(), static_cast<int>(host_strings_.back().size()), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::write_value(size_t pos, const std::string &x)
{
  host_strings_.push_back(x);
  const int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), host_strings_.back().c_str(), static_cast<int>(host_strings_.back().size()), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::write_value(size_t pos, const std::string &x, size_t size)
{
  const auto len = x.size();
  if (size == 0) {
    size = len;
  } else {
    size = (len > size) ? size : len;
  }
  const int ret = sqlite3_bind_text(stmt_, static_cast<int>(++pos), x.data(), static_cast<int>(size), nullptr);
  throw_sqlite_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::write_value(size_t pos, const time &x)
{
  host_strings_.push_back(utils::to_string(x, "%Y-%m-%dT%T.%f"));
  write_value(pos, host_strings_.back());
}

void sqlite_parameter_binder::write_value(size_t pos, const date &x)
{
  host_strings_.push_back(utils::to_string(x, utils::date_format::ISO8601));
  write_value(pos, host_strings_.back());
}

void sqlite_parameter_binder::write_value(size_t pos, const utils::blob &x)
{
  const int ret = sqlite3_bind_blob(stmt_, static_cast<int>(++pos), x.data(), static_cast<int>(x.size()), SQLITE_STATIC);
  throw_sqlite_error(ret, db_, "sqlite3_bind_blob");
}

}