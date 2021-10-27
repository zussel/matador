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
  auto v = std::make_unique<mssql_parameter_binder::value_t>(sizeof(T));
  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(T)];
    *reinterpret_cast<T*>(v->data) = val;
  }
  return v.release();
}

mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, char val)
{
  auto v = std::make_unique<mssql_parameter_binder::value_t>(2);
  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[2];
    v->data[0] = val;
    v->data[1] = '\0';
  }
  return v.release();
}

mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, unsigned char val)
{
  auto v = std::make_unique<mssql_parameter_binder::value_t>(2);
  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[2];
    reinterpret_cast<unsigned char*>(v->data)[0] = val;
    v->data[1] = '\0';
  }
  return v.release();
}

mssql_parameter_binder::value_t* create_bind_value(bool is_null_value, const std::string &val)
{
  auto v = std::make_unique<mssql_parameter_binder::value_t>(SQL_NTS);

  size_t s = val.size();

  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->len = (s == 0 ? 1 : s);
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
  auto v = std::make_unique<mssql_parameter_binder::value_t>(sizeof(T));
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
  auto v = std::make_unique<mssql_parameter_binder::value_t>(val_size + 1);

  if (is_null_value) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[val_size + 1];
#ifdef _MSC_VER
    strncpy_s((char *) v->data, val_size + 1, val, val_size);
#else
    v->data = strncpy((char *)v->data, val, val_size);
#endif
    (v->data)[val_size++] = '\0';
  }
  return v.release();
}

void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, mssql_parameter_binder::value_t *v, size_t index)
{
  SQLLEN buffer_length(0);
  SQLRETURN ret = SQLBindParameter(stmt, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, v->len, 0, v->data, buffer_length, nullptr);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
}

void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, mssql_parameter_binder::value_t *v, unsigned short scale, size_t index)
{
  SQLLEN buffer_length(0);
  SQLRETURN ret = SQLBindParameter(stmt, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, v->len, scale, v->data, buffer_length, nullptr);
  throw_database_error(ret, SQL_HANDLE_STMT, stmt, "mssql");
}

mssql_parameter_binder::mssql_parameter_binder(SQLHANDLE stmt)
  : stmt_(stmt)
{ }

void mssql_parameter_binder::reset()
{
  while (!host_data_.empty()) {
    delete host_data_.back();
    host_data_.pop_back();
  }
  data_to_put_map_.clear();

  index_ = 1;
}

void mssql_parameter_binder::initialize_index(size_t index)
{
  index_ = index + 1;
}

size_t mssql_parameter_binder::next_index()
{
  return index_++;
}

size_t mssql_parameter_binder::current_index() const
{
  return index_ - 1;
}

void mssql_parameter_binder::bind(char i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_CHAR, SQL_CHAR, host_data_.back(), index);
}

void mssql_parameter_binder::bind(short i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_SSHORT, SQL_SMALLINT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(int i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, host_data_.back(), index);
}

void mssql_parameter_binder::bind(long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, host_data_.back(), index);
}

void mssql_parameter_binder::bind(long long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_SBIGINT, SQL_BIGINT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned char i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_CHAR, SQL_CHAR, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned short i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_USHORT, SQL_INTEGER, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned int i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_ULONG, SQL_INTEGER, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_ULONG, SQL_BIGINT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(unsigned long long i, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, i));

  bind_value(stmt_, SQL_C_UBIGINT, SQL_BIGINT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(bool b, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, b));

  bind_value(stmt_, SQL_C_BIT, SQL_BIT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(float d, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, d));

  bind_value(stmt_, SQL_C_FLOAT, SQL_FLOAT, host_data_.back(), index);
}

void mssql_parameter_binder::bind(double d, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, d));

  bind_value(stmt_, SQL_C_DOUBLE, SQL_DOUBLE, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const char *val, size_t s, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, val, s));

  bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const std::string &x, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, x));

  mssql_parameter_binder::value_t *value = host_data_.back();

  data_to_put_map_.insert(std::make_pair(value->data, value));

  bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, index);

  if (!bind_null_) {
    value->result_len = SQL_LEN_DATA_AT_EXEC((SQLLEN)value->len);
  }
}

void mssql_parameter_binder::bind(const std::string &x, size_t s, size_t index)
{
  host_data_.push_back(create_bind_value(bind_null_, x.data(), s));

  bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), index);
}

void mssql_parameter_binder::bind(const matador::time &t, size_t index)
{
  host_data_.push_back(create_bind_value<SQL_TIMESTAMP_STRUCT>(bind_null_));

  host_data_.back()->len = 23;

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

  bind_value(stmt_, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, host_data_.back(), 0, index);
}

void mssql_parameter_binder::bind(const matador::date &d, size_t index)
{
  host_data_.push_back(create_bind_value<SQL_DATE_STRUCT>(bind_null_));

  host_data_.back()->len = 23;

  if (!bind_null_) {
    auto *ts = reinterpret_cast<SQL_DATE_STRUCT *>(host_data_.back()->data);

    ts->year = (SQLSMALLINT) d.year();
    ts->month = (SQLUSMALLINT) d.month();
    ts->day = (SQLUSMALLINT) d.day();
  }

  bind_value(stmt_, SQL_C_TYPE_DATE, SQL_TIMESTAMP, host_data_.back(), 0, index);
}

const std::unordered_map<PTR, mssql_parameter_binder::value_t *> &mssql_parameter_binder::data_to_put_map() const
{
  return data_to_put_map_;
}

}
}