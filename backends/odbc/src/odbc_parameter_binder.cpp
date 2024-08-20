#include "odbc_parameter_binder.hpp"
#include "odbc_error.hpp"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <cstring>

namespace matador::backends::odbc {

namespace detail {

template < class T >
odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, T val);
odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value);
odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, const std::string &val);
odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, const char *val, size_t size);

template < typename Type >
void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, const Type &v, size_t index);
// void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, size_t index);
// void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, unsigned short scale, size_t index);

}

odbc_parameter_binder::odbc_parameter_binder(SQLHANDLE stmt)
: stmt_(stmt)
{}

void odbc_parameter_binder::bind(size_t pos, const char &x)
{
  detail::bind_value(stmt_, SQL_TINYINT, SQL_TINYINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const short &x)
{
  detail::bind_value(stmt_, SQL_C_SSHORT, SQL_SMALLINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const int &x)
{
  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const long &x)
{
  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const long long int &x)
{
  detail::bind_value(stmt_, SQL_C_SBIGINT, SQL_BIGINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const unsigned char &x)
{
  detail::bind_value(stmt_, SQL_C_SHORT, SQL_SMALLINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const unsigned short &x)
{
  detail::bind_value(stmt_, SQL_C_USHORT, SQL_INTEGER, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const unsigned int &x)
{
  detail::bind_value(stmt_, SQL_C_ULONG, SQL_INTEGER, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const unsigned long &x)
{
  detail::bind_value(stmt_, SQL_C_ULONG, SQL_BIGINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const unsigned long long int &x)
{
  detail::bind_value(stmt_, SQL_C_UBIGINT, SQL_BIGINT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const bool &x)
{
  detail::bind_value(stmt_, SQL_C_BIT, SQL_BIT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const float &x)
{
  detail::bind_value(stmt_, SQL_C_FLOAT, SQL_FLOAT, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const double &x)
{
  detail::bind_value(stmt_, SQL_C_DOUBLE, SQL_DOUBLE, x, pos);
}

void odbc_parameter_binder::bind(size_t pos, const char *text)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, text));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::bind(size_t pos, const char *str, size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, str, size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, const std::string &text)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, text));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::bind(size_t pos, const std::string &str, size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, str.data(), size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, const utils::blob &data)
{

}

void odbc_parameter_binder::bind(size_t pos, const time &time)
{

}

void odbc_parameter_binder::bind(size_t pos, const date &date)
{

}

std::optional<std::optional<std::reference_wrapper<odbc_parameter_binder::bounded_value>>> odbc_parameter_binder::get_data_to_put(const PTR ptr) {
  const auto it = data_to_put_map_.find(ptr);
  if (it == data_to_put_map_.end()) {
    return std::nullopt;
  }

  return std::reference_wrapper(it->second);
}

namespace detail {
template < class T >
odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, T val)
{
  odbc_parameter_binder::bounded_value v;
  if (is_null_value) {
    v.data = nullptr;
    v.len = SQL_NULL_DATA;
  } else {
    v.data.reset(new char[sizeof(T)]);
    *reinterpret_cast<T*>(v.data.get()) = val;
  }
  return v;
}

template < class T >
odbc_parameter_binder::bounded_value create_bind_value(const bool is_null_value)
{
  odbc_parameter_binder::bounded_value v;
  if (is_null_value) {
    v.data = nullptr;
    v.len = SQL_NULL_DATA;
  } else {
    v.data.reset(new char[sizeof(T)]);
  }
  return v;
}

odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, const std::string &val)
{
  odbc_parameter_binder::bounded_value v;
  size_t s = val.size();

  if (is_null_value) {
    v.data = nullptr;
    v.len = SQL_NULL_DATA;
  } else {
    v.len = static_cast<SQLLEN>(s == 0 ? 1 : s);
    v.data.reset(new char[s + 1]);
#ifdef _MSC_VER
    strncpy_s(v.data.get(), s + 1, val.c_str(), s);
#else
    strncpy(v.data.get(), val.c_str(), s);
#endif
    (v.data)[s++] = '\0';
  }

  return v;
}

odbc_parameter_binder::bounded_value create_bind_value(bool is_null_value, const char *val, size_t size)
{
  size_t val_size = strlen(val);
  if (size > 0) {
    val_size = (val_size < size ? val_size : size);
  }
  odbc_parameter_binder::bounded_value v;

  if (is_null_value) {
    v.data = nullptr;
    v.len = SQL_NULL_DATA;
  } else {
    v.data.reset(new char[val_size + 1]);
#ifdef _MSC_VER
    strncpy_s((char *) v.data.get(), val_size + 1, val, val_size);
#else
    strncpy((char *)v.data.get(), val, val_size);
#endif
    (v.data)[val_size++] = '\0';
  }
  return v;
}

// void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, size_t index)
// {
//   constexpr SQLLEN buffer_length(0);
//   const SQLRETURN ret = SQLBindParameter(stmt,
//                                          index,
//                                          SQL_PARAM_INPUT,
//                                          static_cast<SQLSMALLINT>(ctype),
//                                          static_cast<SQLSMALLINT>(type),
//                                          v.len,
//                                          0,
//                                          v.data.get(),
//                                          buffer_length,
//                                          nullptr);
//   throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
// }

template < typename Type >
void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, const Type &v, size_t index)
{
  SQLLEN value_len{0};
  const SQLRETURN ret = SQLBindParameter(stmt,
                                         static_cast<SQLUSMALLINT>(index),
                                         SQL_PARAM_INPUT,
                                         static_cast<SQLSMALLINT>(ctype),
                                         static_cast<SQLSMALLINT>(type),
                                         0,
                                         0,
                                         static_cast<void *>(const_cast<Type*>(&v)),
                                         0,
                                         &value_len);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
}

// void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, unsigned short scale, size_t index)
// {
//   constexpr SQLLEN buffer_length(0);
//   const SQLRETURN ret = SQLBindParameter(stmt,
//                                          index,
//                                          SQL_PARAM_INPUT,
//                                          static_cast<SQLSMALLINT>(ctype),
//                                          static_cast<SQLSMALLINT>(type),
//                                          v.len,
//                                          static_cast<SQLSMALLINT>(scale),
//                                          v.data.get(),
//                                          buffer_length,
//                                          nullptr);
//   throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
// }

}
}
