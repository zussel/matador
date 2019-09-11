//
// Created by sascha on 06.09.19.
//

#include <cstring>
#include "matador/db/mysql/mysql_parameter_binder.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

namespace matador {

namespace mysql {

template < class T >
void bind_value(enum_field_types type, T value, MYSQL_BIND &bind, my_bool &is_null)
{
  if (bind.buffer == nullptr) {
    // allocating memory
    bind.buffer = new char[sizeof(T)];
    bind.buffer_type = type;
    bind.buffer_length = sizeof(T);
    bind.is_null = &is_null;
    bind.is_unsigned = std::is_unsigned<T>::value;
  }
  *static_cast<T*>(bind.buffer) = value;
  is_null = false;
}

void bind_value(enum_field_types type, const char *value, size_t, MYSQL_BIND &bind, my_bool &is_null)
{
  std::size_t len(strlen(value) + 1);
  if (bind.buffer_length < len) {
    // reallocate memory
    delete [] static_cast<char*>(bind.buffer);
    bind.buffer = nullptr;
    bind.buffer_length = 0;
    bind.buffer_type = type;
    bind.is_null = &is_null;
  }
  if (bind.buffer == nullptr) {
    // allocating memory
    bind.buffer = new char[len];
    memset(bind.buffer, 0, len);
  }
  bind.buffer_length = (unsigned long)(len - 1);
#ifdef _MSC_VER
  strncpy_s(static_cast<char*>(bind.buffer), len, value, _TRUNCATE);
#else
  strncpy(static_cast<char*>(bind.buffer), value, len);
#endif
  is_null = false;
}

void bind_value(enum_field_types type, const matador::date &x, MYSQL_BIND &bind, my_bool &is_null)
{
  if (bind.buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    bind.buffer = new char[s];
    bind.buffer_length = (unsigned long)s;
    bind.is_null = &is_null;
    bind.buffer_type = type;
    bind.length = nullptr;
  }
  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
  is_null = false;
  auto *mt = static_cast<MYSQL_TIME*>(bind.buffer);
  mt->day = (unsigned int)x.day();
  mt->month = (unsigned int)x.month();
  mt->year = (unsigned int)x.year();
  mt->time_type  = MYSQL_TIMESTAMP_DATE;
}

void bind_value(enum_field_types type, const matador::time &x, MYSQL_BIND &bind, my_bool &is_null)
{
  if (bind.buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    bind.buffer = new char[s];
    bind.buffer_length = (unsigned long)s;
    bind.buffer_type = type;
    bind.length = nullptr;
    bind.is_null = &is_null;
  }
  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
  is_null = false;
  auto *mt = static_cast<MYSQL_TIME*>(bind.buffer);
  mt->day = (unsigned int)x.day();
  mt->month = (unsigned int)x.month();
  mt->year = (unsigned int)x.year();
  mt->hour = (unsigned int)x.hour();
  mt->minute = (unsigned int)x.minute();
  mt->second = (unsigned int)x.second();
  mt->second_part = (unsigned long)x.milli_second() * 1000;
  mt->time_type  = MYSQL_TIMESTAMP_DATETIME;
}

void mysql_parameter_binder::reset()
{
  index_ = 0;
}

void mysql_parameter_binder::initialize_index(size_t index)
{
  index_ = index;
}

size_t mysql_parameter_binder::next_index()
{
  return index_++;
}

size_t mysql_parameter_binder::current_index() const
{
  return index_;
}

void mysql_parameter_binder::bind(char i, size_t index)
{
  bind_value(MYSQL_TYPE_TINY, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(short i, size_t index)
{
  bind_value(MYSQL_TYPE_SHORT, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(int i, size_t index)
{
  bind_value(MYSQL_TYPE_LONG, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(long i, size_t index)
{
  bind_value(MYSQL_TYPE_LONG, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(unsigned char i, size_t index)
{
  bind_value(MYSQL_TYPE_TINY, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(unsigned short i, size_t index)
{
  bind_value(MYSQL_TYPE_SHORT, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(unsigned int i, size_t index)
{
  bind_value(MYSQL_TYPE_LONG, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(unsigned long i, size_t index)
{
  bind_value(MYSQL_TYPE_LONG, i, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(bool b, size_t index)
{
  bind_value(MYSQL_TYPE_TINY, b, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(float d, size_t index)
{
  bind_value(MYSQL_TYPE_FLOAT, d, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(double d, size_t index)
{
  bind_value(MYSQL_TYPE_DOUBLE, d, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(const char *x, size_t size, size_t index)
{
  bind_value(MYSQL_TYPE_VAR_STRING, x, size, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(const std::string &x, size_t index)
{
  bind_value(MYSQL_TYPE_STRING, x.data(), x.size(), bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(const std::string &x, size_t size, size_t index)
{
  bind_value(MYSQL_TYPE_VAR_STRING, x.data(), size, bind_[index], is_null_vector[index]);
}

void mysql_parameter_binder::bind(const matador::time &x, size_t index)
{
  if (mysql::version < 50604) {
    // before mysql version 5.6.4 datetime
    // doesn't support fractional seconds
    // so we use a datetime string here
    std::string tstr = to_string(x, "%FT%T");
    bind_value(MYSQL_TYPE_VAR_STRING, tstr.c_str(), tstr.size(), bind_[index], is_null_vector[index]);
  } else {
    bind_value(MYSQL_TYPE_DATE, x, bind_[index], is_null_vector[index]);
  }

}

void mysql_parameter_binder::bind(const matador::date &x, size_t index)
{
  bind_value(MYSQL_TYPE_TIMESTAMP, x, bind_[index], is_null_vector[index]);
}
}
}
