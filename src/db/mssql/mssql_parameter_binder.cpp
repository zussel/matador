//
// Created by sascha on 30.08.19.
//

#include "matador/db/mssql/mssql_parameter_binder.hpp"
#include "matador/db/mssql/mssql_statement.hpp"

#include "matador/utils/memory.hpp"

#include "matador/sql/types.hpp"

#include <sql.h>

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

mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, const std::string &val)
{
  auto v = matador::make_unique<mssql_parameter_binder::value_t>(SQL_NTS);

  size_t s = val.size();

  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->len = s;
    v->data = new char[s + 1];
#ifdef _MSC_VER
    strncpy_s(v->data, s + 1, val.c_str(), s);
#else
    v->data = strncpy(v->data, val.c_str(), s);
#endif
    (v->data)[s++] = '\0';
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

mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, const char *val, size_t size)
{
  size_t val_size = strlen(val);
  val_size = (val_size < size ? val_size : size);
  auto v = matador::make_unique<mssql_parameter_binder::value_t>(val_size);

  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->len = val_size;
    v->data = new char[val_size + 1];
#ifdef _MSC_VER
    strncpy_s((char *) v->data, val_size + 1, val, val_size);
#else
    v->data = strcpy((char *)v->data, val);
#endif
    (v->data)[val_size++] = '\0';
  }
  return v.release();
}

template < class T >
void bind_value(SQLHANDLE stmt, mssql_parameter_binder::value_t *v, size_t index)
{
  data_type dt = data_type_traits<T>::type();
  auto ctype = (SQLUSMALLINT) mssql_statement::type2int(dt);
  auto type = (SQLUSMALLINT) mssql_statement::type2sql(dt);
  SQLRETURN ret = SQLBindParameter(stmt, (SQLUSMALLINT) index, SQL_PARAM_INPUT, ctype, type, 1, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt, "mssql", "couldn't bind parameter");
}

mssql_parameter_binder::mssql_parameter_binder(SQLHANDLE stmt)
  : stmt_(stmt)
{ }

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

void mssql_parameter_binder::bind(const char *val, size_t s, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, val, s));

  bind_value<const char*>(stmt_, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const std::string &x, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, x));

  mssql_parameter_binder::value_t *value = host_data_.back();

  data_to_put_map_.insert(std::make_pair(value->data, value));

  bind_value<std::string>(stmt_, value, index);

  if (!bind_null_) {
    value->result_len = SQL_LEN_DATA_AT_EXEC((SQLLEN)value->len);
  }
}

void mssql_parameter_binder::bind(const std::string &x, size_t s, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, x.data(), s));

  bind_value<const char*>(stmt_, host_data_.back(), index);
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

  bind_value<matador::time>(stmt_, host_data_.back(), index);
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

  bind_value<matador::date>(stmt_, host_data_.back(), index);
}

}
}