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

void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, size_t index);
void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, unsigned short scale, size_t index);

}

odbc_parameter_binder::odbc_parameter_binder(SQLHANDLE stmt)
: stmt_(stmt)
{}

void odbc_parameter_binder::bind(size_t pos, char i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_TINYINT, SQL_TINYINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, short i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_SSHORT, SQL_SMALLINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, int i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, long i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, long long i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_SBIGINT, SQL_BIGINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, unsigned char i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_SHORT, SQL_SMALLINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, unsigned short i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_USHORT, SQL_INTEGER, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, unsigned int i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_ULONG, SQL_INTEGER, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, unsigned long i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_ULONG, SQL_BIGINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, unsigned long long i)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, i));

  detail::bind_value(stmt_, SQL_C_UBIGINT, SQL_BIGINT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, bool b)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, b));

  detail::bind_value(stmt_, SQL_C_BIT, SQL_BIT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, float d)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, d));

  detail::bind_value(stmt_, SQL_C_FLOAT, SQL_FLOAT, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, double d)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, d));

  detail::bind_value(stmt_, SQL_C_DOUBLE, SQL_DOUBLE, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, const char *str)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, str));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::bind(size_t pos, const char *x, size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, x, size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, const std::string &str)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, str));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::bind(size_t pos, const std::string &x, size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, x.data(), size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::bind(size_t pos, const utils::blob &blob)
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

// std::unique_ptr<odbc_parameter_binder::bounded_value> create_bind_value(const bool is_null_value, char val)
// {
//   auto v = std::make_unique<odbc_parameter_binder::bounded_value>(2);
//   if (is_null_value) {
//     v->data = nullptr;
//     v->len = SQL_NULL_DATA;
//   } else {
//     v->data.reset(new char[2]);
//     v->data[0] = val;
//     v->data[1] = '\0';
//   }
//   return v;
// }
//
// std::unique_ptr<odbc_parameter_binder::bounded_value> create_bind_value(const bool is_null_value, unsigned char val)
// {
//   auto v = std::make_unique<odbc_parameter_binder::bounded_value>(2);
//   if (is_null_value) {
//     v->data = nullptr;
//     v->len = SQL_NULL_DATA;
//   } else {
//     v->data.reset(new char[2]);
//     reinterpret_cast<unsigned char*>(v->data)[0] = val;
//     v->data[1] = '\0';
//   }
//   return v;
// }

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

void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, size_t index)
{
  constexpr SQLLEN buffer_length(0);
  const SQLRETURN ret = SQLBindParameter(stmt,
                                         index,
                                         SQL_PARAM_INPUT,
                                         static_cast<SQLSMALLINT>(ctype),
                                         static_cast<SQLSMALLINT>(type),
                                         v.len,
                                         0,
                                         v.data.get(),
                                         buffer_length,
                                         nullptr);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
}

void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, unsigned short scale, size_t index)
{
  constexpr SQLLEN buffer_length(0);
  const SQLRETURN ret = SQLBindParameter(stmt,
                                         index,
                                         SQL_PARAM_INPUT,
                                         static_cast<SQLSMALLINT>(ctype),
                                         static_cast<SQLSMALLINT>(type),
                                         v.len,
                                         static_cast<SQLSMALLINT>(scale),
                                         v.data.get(),
                                         buffer_length,
                                         nullptr);
  throw_odbc_error(ret, SQL_HANDLE_STMT, stmt, "odbc");
}

}
}
