//
// Created by sascha on 30.08.19.
//

#include "matador/db/mssql/mssql_parameter_binder.hpp"

#include "matador/sql/types.hpp"

#include <sql.h>
#include <matador/db/mssql/mssql_statement.hpp>
#include <matador/utils/memory.hpp>

namespace matador {

namespace mssql {

template < class T >
mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, T val)
{
  auto v = matador::make_unique<mssql_parameter_binder::value_t>(sizeof(T));
  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(T)];
    *reinterpret_cast<T*>(v->data) = val;
  }
  return v.release();
}

template < class T >
mssql_parameter_binder::value_t* create_bind_value(bool is_null_value)
{
  auto v = matador::make_unique<mssql_parameter_binder::value_t>(sizeof(T));
  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(T)];
  }
  return v.release();
}

template < class T >
void bind_value(SQLHANDLE stmt, mssql_parameter_binder::value_t *v, size_t index)
{
  auto ctype = (SQLUSMALLINT) mssql_statement::type2int(data_type_traits<T>::type());
  auto type = (SQLUSMALLINT) mssql_statement::type2sql(data_type_traits<T>::type());
  SQLRETURN ret = SQLBindParameter(stmt, (SQLUSMALLINT) index, SQL_PARAM_INPUT, ctype, type, 1, 0, v->data, v->len, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "couldn't bind parameter");
}

void mssql_parameter_binder::bind(char i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<char>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(short i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<short>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(int i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<int>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<long>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned char i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<unsigned char>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned short i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<unsigned short>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned int i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<unsigned int>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value<unsigned long>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(bool b, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, b));

  bind_value<bool>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(float d, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, d));

  bind_value<float>(stmt_, host_data_.back(), index);

}

void mssql_parameter_binder::bind(double d, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, d));

  bind_value<double>(stmt_, host_data_.back(), index);

}

void mssql_parameter_binder::bind(const char *string, size_t size, size_t)
{

}

void mssql_parameter_binder::bind(const std::string &string, size_t)
{

}

void mssql_parameter_binder::bind(const std::string &x, size_t s, size_t)
{

}

void mssql_parameter_binder::bind(const matador::time &t, size_t index)
{
  host_data_.push_back(create_bind_value<SQL_TIMESTAMP_STRUCT>(bind_null_));

  if (!bind_null_) {
    auto *ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(host_data_.back()->data);

    ts->year = (SQLSMALLINT) t.year();
    ts->month = (SQLUSMALLINT) t.month();
    ts->day = (SQLUSMALLINT) t.day();
    ts->hour = (SQLUSMALLINT) t.hour();
    ts->minute = (SQLUSMALLINT) t.minute();
    ts->second = (SQLUSMALLINT) t.second();
    ts->fraction = (SQLUINTEGER) t.milli_second() * 1000 * 1000;
  }

  bind_value<unsigned long>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const matador::date &d, size_t index)
{
  host_data_.push_back(create_bind_value<SQL_DATE_STRUCT>(bind_null_));

  if (!bind_null_) {
    auto *ts = reinterpret_cast<SQL_DATE_STRUCT *>(host_data_.back()->data);

    ts->year = (SQLSMALLINT) d.year();
    ts->month = (SQLUSMALLINT) d.month();
    ts->day = (SQLUSMALLINT) d.day();
  }

  bind_value<unsigned long>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const matador::basic_identifier &x, size_t)
{

}

void mssql_parameter_binder::bind(const matador::identifiable_holder &x, cascade_type type, size_t)
{

}

}
}