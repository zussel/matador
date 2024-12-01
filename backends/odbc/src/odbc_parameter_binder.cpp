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
void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, const Type &v, size_t len, size_t index);
void bind_value(SQLHANDLE stmt, SQLSMALLINT ctype, SQLSMALLINT type, const odbc_parameter_binder::bounded_value &v, size_t index);
// void bind_value(SQLHANDLE stmt, SQLUSMALLINT ctype, SQLUSMALLINT type, odbc_parameter_binder::bounded_value &v, unsigned short scale, size_t index);

}

odbc_parameter_binder::odbc_parameter_binder(const SQLHANDLE stmt)
: stmt_(stmt)
{}

void odbc_parameter_binder::write_value(size_t pos, const char &x)
{
  detail::bind_value(stmt_, SQL_TINYINT, SQL_TINYINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const short &x)
{
  detail::bind_value(stmt_, SQL_C_SSHORT, SQL_SMALLINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const int &x)
{
  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const long &x)
{
  detail::bind_value(stmt_, SQL_C_SLONG, SQL_INTEGER, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const long long int &x)
{
  detail::bind_value(stmt_, SQL_C_SBIGINT, SQL_BIGINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const unsigned char &x)
{
  detail::bind_value(stmt_, SQL_C_SHORT, SQL_SMALLINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const unsigned short &x)
{
  detail::bind_value(stmt_, SQL_C_USHORT, SQL_INTEGER, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const unsigned int &x)
{
  detail::bind_value(stmt_, SQL_C_ULONG, SQL_INTEGER, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const unsigned long &x)
{
  detail::bind_value(stmt_, SQL_C_ULONG, SQL_BIGINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const unsigned long long int &x)
{
  detail::bind_value(stmt_, SQL_C_UBIGINT, SQL_BIGINT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const bool &x)
{
  detail::bind_value(stmt_, SQL_C_BIT, SQL_BIT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const float &x)
{
  detail::bind_value(stmt_, SQL_C_FLOAT, SQL_FLOAT, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const double &x)
{
  detail::bind_value(stmt_, SQL_C_DOUBLE, SQL_DOUBLE, x, 0, pos);
}

void odbc_parameter_binder::write_value(size_t pos, const char *text)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, text));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, static_cast<const char*>(value.data.get()), value.len, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::write_value(const size_t pos, const char *x, const size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, x, size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::write_value(size_t pos, const std::string &text)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, text));

  bounded_value &value = host_data_.back();

  data_to_put_map_.insert({value.data.get(), value});

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_LONGVARCHAR, value, pos);

  if (!bind_null_) {
    value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  }
}

void odbc_parameter_binder::write_value(size_t pos, const std::string &str, size_t size)
{
  host_data_.push_back(detail::create_bind_value(bind_null_, str.data(), size));

  detail::bind_value(stmt_, SQL_C_CHAR, SQL_VARCHAR, host_data_.back(), pos);
}

void odbc_parameter_binder::write_value(size_t pos, const time &time)
{

}

void odbc_parameter_binder::write_value(size_t pos, const date &date)
{

}

void odbc_parameter_binder::write_value(size_t pos, const utils::blob &x)
{
    host_data_.push_back(detail::create_bind_value(bind_null_, reinterpret_cast<const char*>(x.data()), x.size()));

    detail::bind_value(stmt_, SQL_C_BINARY, SQL_VARBINARY, host_data_.back(), pos);
}

void odbc_parameter_binder::write_value( size_t pos, const utils::value& x, size_t size )
{

}

void odbc_parameter_binder::reset()
{
  host_data_.clear();
}

// odbc_parameter_binder::bounded_value detail::create_bind_value( bool is_null_value ) {}

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

odbc_parameter_binder::bounded_value create_bind_value(const bool is_null_value, const char *val, const size_t size)
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
    v.len = static_cast<SQLLEN>(val_size);
#ifdef _MSC_VER
    strncpy_s((char *) v.data.get(), val_size + 1, val, val_size);
#else
    strncpy((char *)v.data.get(), val, val_size);
#endif
    (v.data)[val_size++] = '\0';
  }
  return v;
}

void bind_value(const SQLHANDLE stmt, const SQLSMALLINT ctype, const SQLSMALLINT type, const odbc_parameter_binder::bounded_value &v, const size_t index)
{
  const SQLRETURN ret = SQLBindParameter(stmt,
                                         static_cast<SQLUSMALLINT>(index),
                                         SQL_PARAM_INPUT,
                                         ctype,
                                         type,
                                         v.len,
                                         0,
                                         v.data.get(),
                                         v.len,
                                         nullptr);
  if (!is_succeeded_or_no_data(ret)) {
    // Todo: handle odbc error
    make_error(sql::sql_error_code::BIND_FAILED, ret, SQL_HANDLE_STMT, stmt);
  }
}

template < typename Type >
void bind_value(SQLHANDLE stmt, const SQLUSMALLINT ctype, const SQLUSMALLINT type, const Type &v, const size_t len, const size_t index)
{
  SQLLEN value_len{0};
  const SQLRETURN ret = SQLBindParameter(stmt,
                                         static_cast<SQLUSMALLINT>(index),
                                         SQL_PARAM_INPUT,
                                         static_cast<SQLSMALLINT>(ctype),
                                         static_cast<SQLSMALLINT>(type),
                                         len,
                                         0,
                                         &const_cast<Type&>(v),
                                         0,
                                         &value_len);
  if (!is_succeeded_or_no_data(ret)) {
    // Todo: handle odbc error
    make_error(sql::sql_error_code::BIND_FAILED, ret, SQL_HANDLE_STMT, stmt);
  }
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
