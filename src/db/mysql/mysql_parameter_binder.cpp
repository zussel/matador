//
// Created by sascha on 06.09.19.
//

#include "matador/db/mysql/mysql_parameter_binder.hpp"

namespace matador {

namespace mysql {

template < class T >
void bind_value(enum_field_types type, T value, MYSQL_BIND &bind, my_bool &is_null)
{
  if (bind.buffer == nullptr) {
    // allocating memory
    bind.buffer = new char[sizeof(T)];
  }
  *static_cast<T*>(bind.buffer) = value;
  bind.buffer_type = type;
  bind.buffer_length = sizeof(T);
  is_null = false;
  bind.is_null = &is_null;
  bind.is_unsigned = std::is_unsigned<T>::value;
}

void mysql_parameter_binder::reset()
{

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

void mysql_parameter_binder::bind(const char *string, size_t size, size_t index)
{

}

void mysql_parameter_binder::bind(const std::string &string, size_t index)
{

}

void mysql_parameter_binder::bind(const std::string &x, size_t size, size_t index)
{

}

void mysql_parameter_binder::bind(const matador::time &time, size_t index)
{

}

void mysql_parameter_binder::bind(const matador::date &date, size_t index)
{

}
}
}
