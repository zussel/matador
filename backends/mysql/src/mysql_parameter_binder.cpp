#include "mysql_parameter_binder.hpp"

#include <cstring>

namespace matador::backends::mysql {

namespace detail {

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

//void bind_value(enum_field_types type, const matador::date &x, MYSQL_BIND &bind, my_bool &is_null)
//{
//  if (bind.buffer == nullptr) {
//    size_t s = sizeof(MYSQL_TIME);
//    bind.buffer = new char[s];
//    bind.buffer_length = (unsigned long)s;
//    bind.is_null = &is_null;
//    bind.buffer_type = type;
//    bind.length = nullptr;
//  }
//  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
//  is_null = false;
//  auto *mt = static_cast<MYSQL_TIME*>(bind.buffer);
//  mt->day = (unsigned int)x.day();
//  mt->month = (unsigned int)x.month();
//  mt->year = (unsigned int)x.year();
//  mt->time_type  = MYSQL_TIMESTAMP_DATE;
//}
//
//void bind_value(enum_field_types type, const matador::time &x, MYSQL_BIND &bind, my_bool &is_null)
//{
//  if (bind.buffer == nullptr) {
//    size_t s = sizeof(MYSQL_TIME);
//    bind.buffer = new char[s];
//    bind.buffer_length = (unsigned long)s;
//    bind.buffer_type = type;
//    bind.length = nullptr;
//    bind.is_null = &is_null;
//  }
//  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
//  is_null = false;
//  auto *mt = static_cast<MYSQL_TIME*>(bind.buffer);
//  mt->day = (unsigned int)x.day();
//  mt->month = (unsigned int)x.month();
//  mt->year = (unsigned int)x.year();
//  mt->hour = (unsigned int)x.hour();
//  mt->minute = (unsigned int)x.minute();
//  mt->second = (unsigned int)x.second();
//  mt->second_part = (unsigned long)x.milli_second() * 1000;
//  mt->time_type  = MYSQL_TIMESTAMP_DATETIME;
//}

}

mysql_parameter_binder::mysql_parameter_binder(size_t size)
: bind_params_(size)
, is_null_vector(size)
, info_(size)
{}

void mysql_parameter_binder::bind(size_t pos, char i)
{
  detail::bind_value(MYSQL_TYPE_TINY, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, short i)
{
  detail::bind_value(MYSQL_TYPE_SHORT, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, int i)
{
  detail::bind_value(MYSQL_TYPE_LONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, long i)
{
  detail::bind_value(MYSQL_TYPE_LONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, long long int i)
{
  detail::bind_value(MYSQL_TYPE_LONGLONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, unsigned char i)
{
  detail::bind_value(MYSQL_TYPE_TINY, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, unsigned short i)
{
  detail::bind_value(MYSQL_TYPE_SHORT, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, unsigned int i)
{
  detail::bind_value(MYSQL_TYPE_LONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, unsigned long i)
{
  detail::bind_value(MYSQL_TYPE_LONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, unsigned long long int i)
{
  detail::bind_value(MYSQL_TYPE_LONGLONG, i, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, bool b)
{
  detail::bind_value(MYSQL_TYPE_TINY, b, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, float d)
{
  detail::bind_value(MYSQL_TYPE_FLOAT, d, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, double d)
{
  detail::bind_value(MYSQL_TYPE_DOUBLE, d, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, const char *str)
{
  detail::bind_value(MYSQL_TYPE_STRING, str, strlen(str), bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, const char *str, size_t size)
{
  detail::bind_value(MYSQL_TYPE_VAR_STRING, str, size, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, const std::string &str)
{
  detail::bind_value(MYSQL_TYPE_STRING, str.data(), str.size(), bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, const std::string &str, size_t size)
{
  detail::bind_value(MYSQL_TYPE_VAR_STRING, str.data(), size, bind_params_[pos], is_null_vector[pos].is_null);
}

void mysql_parameter_binder::bind(size_t pos, const utils::blob &blob)
{

}

std::vector<MYSQL_BIND> &mysql_parameter_binder::bind_params()
{
  return bind_params_;
}

void mysql_parameter_binder::bind( size_t pos, const time& x )
{
  if (bind.buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    bind.buffer = new char[s];
    bind.buffer_length = (unsigned long)s;
    bind.buffer_type = MYSQL_TYPE_TIMESTAMP;
    bind.length = nullptr;
    bind.is_null = &is_null;
  }
  memset(bind.buffer, 0, sizeof(MYSQL_TIME));
  // is_null = false;
  auto *mt = static_cast<MYSQL_TIME*>(bind.buffer);
  mt->day = static_cast<unsigned int>(x.day());
  mt->month = static_cast<unsigned int>(x.month());
  mt->year = static_cast<unsigned int>(x.year());
  mt->hour = static_cast<unsigned int>(x.hour());
  mt->minute = static_cast<unsigned int>(x.minute());
  mt->second = static_cast<unsigned int>(x.second());
  mt->second_part = x.milli_second() * 1000;
  mt->time_type  = MYSQL_TIMESTAMP_DATETIME;
}

void mysql_parameter_binder::bind( size_t pos, const date& date )
{

}

}