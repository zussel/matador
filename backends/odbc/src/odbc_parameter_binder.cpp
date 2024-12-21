#include "odbc_parameter_binder.hpp"
#include "odbc_error.hpp"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <cstring>

namespace matador::backends::odbc {

odbc_parameter_binder::odbc_parameter_binder(const SQLHANDLE stmt, std::vector<odbc_bound_value> &&bound_values)
: stmt_(stmt)
, bound_values_(std::move(bound_values))
{}

template<typename Type>
void copy_value(std::vector<unsigned char> &data, Type &value)
{
  memcpy(data.data(), &value, sizeof(Type));
}

void odbc_parameter_binder::write_value(const size_t pos, const char &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const short &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const int &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const long &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const long long int &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const unsigned char &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const unsigned short &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const unsigned int &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const unsigned long &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const unsigned long long int &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const bool &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const float &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const double &x)
{
  copy_value(bound_values_[pos].data, x);
}

void odbc_parameter_binder::write_value(const size_t pos, const char *x)
{
  bound_values_[pos].data.assign(x, x + strlen(x) + 1);

  // bounded_value &value = host_data_.back();
  // data_to_put_map_.insert({value.data.get(), value});
  // if (!bind_null_) {
    // value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  // }
}

void odbc_parameter_binder::write_value(const size_t pos, const char *x, const size_t size)
{
  const size_t len = strlen(x) + 1;
  bound_values_[pos].data.assign(x, x + std::min(len, size));
}

void odbc_parameter_binder::write_value(const size_t pos, const std::string &x)
{
  std::copy(x.begin(), x.end(), bound_values_[pos].data.begin());
  bound_values_[pos].data.push_back('\0');

  // bounded_value &value = host_data_.back();
  // data_to_put_map_.insert({value.data.get(), value});
  // if (!bind_null_) {
    // value.result_len = SQL_LEN_DATA_AT_EXEC(value.len);
  // }
}

void odbc_parameter_binder::write_value(const size_t pos, const std::string &x, size_t /*size*/)
{
  std::copy(x.begin(), x.end(), bound_values_[pos].data.begin());
}

void odbc_parameter_binder::write_value(const size_t pos, const time &time)
{

}

void odbc_parameter_binder::write_value(const size_t pos, const date &date)
{

}

void odbc_parameter_binder::write_value(const size_t pos, const utils::blob &x)
{
  std::copy(x.begin(), x.end(), bound_values_[pos].data.begin());
}

void odbc_parameter_binder::write_value( const size_t pos, const utils::value& x, size_t size )
{

}

void odbc_parameter_binder::reset()
{
  host_data_.clear();
}

std::optional<std::optional<std::reference_wrapper<odbc_parameter_binder::bounded_value>>> odbc_parameter_binder::get_data_to_put(const PTR ptr) {
  const auto it = data_to_put_map_.find(ptr);
  if (it == data_to_put_map_.end()) {
    return std::nullopt;
  }

  return std::reference_wrapper(it->second);
}

}
