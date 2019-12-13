//
// Created by sascha on 08.09.19.
//

#include <cstring>
#include "matador/db/sqlite/sqlite_parameter_binder.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

namespace matador {

namespace sqlite {

sqlite_parameter_binder::sqlite_parameter_binder(sqlite3 *db, sqlite3_stmt *stmt)
  : db_(db), stmt_(stmt)
{}

void sqlite_parameter_binder::reset()
{
  host_strings_.clear();
  index_ = 1;
}

void sqlite_parameter_binder::initialize_index(size_t index)
{
  index_ = index + 1;
}

size_t sqlite_parameter_binder::next_index()
{
  return index_++;
}

size_t sqlite_parameter_binder::current_index() const
{
  return index_ - 1;
}

void sqlite_parameter_binder::bind(char i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(short i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(int i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(long i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(long long i, size_t index)
{
  int ret = sqlite3_bind_int64(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(unsigned char i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(unsigned short i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(unsigned int i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(unsigned long i, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(unsigned long long i, size_t index)
{
  int ret = sqlite3_bind_int64(stmt_, (int)index, i);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(bool b, size_t index)
{
  int ret = sqlite3_bind_int(stmt_, (int)index, b);
  throw_error(ret, db_, "sqlite3_bind_int");
}

void sqlite_parameter_binder::bind(float d, size_t index)
{
  int ret = sqlite3_bind_double(stmt_, (int)index, d);
  throw_error(ret, db_, "sqlite3_bind_double");
}

void sqlite_parameter_binder::bind(double d, size_t index)
{
  int ret = sqlite3_bind_double(stmt_, (int)index, d);
  throw_error(ret, db_, "sqlite3_bind_double");
}

void sqlite_parameter_binder::bind(const char *x, size_t len, size_t index)
{
  auto size = strlen(x);
  len = (size > len) ? len : size;
  int ret = sqlite3_bind_text(stmt_, (int)index, x, (int)len, nullptr);
  throw_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(const std::string &x, size_t index)
{
  int ret = sqlite3_bind_text(stmt_, (int)index, x.c_str(), (int)x.size(), nullptr);
  throw_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(const std::string &x, size_t len, size_t index)
{
  auto size = x.size();
  len = (size > len) ? len : size;
  int ret = sqlite3_bind_text(stmt_, (int)index, x.data(), (int)len, nullptr);
  throw_error(ret, db_, "sqlite3_bind_text");
}

void sqlite_parameter_binder::bind(const matador::time &x, size_t index)
{
  auto time_string = std::make_shared<std::string>(matador::to_string(x, "%Y-%m-%dT%T.%f"));
  bind(*time_string, index);
  host_strings_.push_back(time_string);
}

void sqlite_parameter_binder::bind(const matador::date &x, size_t index)
{
  // format time to ISO8601
  auto date_string = std::make_shared<std::string>(matador::to_string(x, date_format::ISO8601));
  bind(*date_string, index);
  host_strings_.push_back(date_string);
}

}
}