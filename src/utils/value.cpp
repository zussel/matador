#include "matador/utils/value.hpp"

#include <cstring>

namespace matador::utils {
namespace detail {

size_t determine_size(const std::string &val)
{
  return val.size();
}

size_t determine_size(const char *val)
{
  return strlen(val);
}

size_t determine_size(const utils::blob &val)
{
  return val.size();
}

}
value::value(data_type data_type, size_t size)
: size_(size)
, type_(data_type)
{
  utils::initialize_by_data_type(type_, value_);
}

value::value(value &&x) noexcept
  : value_(std::move(x.value_))
  , type_(x.type_)
{
  x.value_ = nullptr;
  x.type_ = data_type::type_unknown;
}

value &value::operator=(value &&x) noexcept
{
  value_ = std::move(x.value_);
  type_ = x.type_;
  x.value_ = nullptr;
  x.type_ = data_type::type_unknown;

  return *this;
}

std::string value::str() const
{
  return as<std::string>().value();
}

size_t value::size() const
{
  return size_;
}

data_type value::type() const
{
  return type_;
}

bool value::is_integer() const
{
  return type_ >= data_type::type_char && type_ <= data_type::type_unsigned_long_long;
}

bool value::is_floating_point() const
{
  return type_ == data_type::type_float || type_ == data_type::type_double;
}

bool value::is_bool() const
{
  return type_ == data_type::type_bool;
}

bool value::is_string() const
{
  return type_ == data_type::type_text;
}

bool value::is_varchar() const
{
  return type_ == data_type::type_varchar || type_ == data_type::type_char_pointer;
}

bool value::is_date() const
{
  return type_ == data_type::type_date;
}

bool value::is_time() const
{
  return type_ == data_type::type_time;
}

bool value::is_blob() const
{
  return type_ == data_type::type_blob;
}

bool value::is_null() const
{
  return type_ == data_type::type_null;
}

bool value::is_unknown() const
{
  return type_ == data_type::type_unknown;
}

}