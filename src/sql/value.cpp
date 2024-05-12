#include "matador/sql/value.hpp"
#include "matador/sql/token_visitor.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

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

std::vector<data_type> index2type {
  data_type::type_char,
  data_type::type_short,
  data_type::type_int,
  data_type::type_long,
  data_type::type_long_long,
  data_type::type_unsigned_char,
  data_type::type_unsigned_short,
  data_type::type_unsigned_int,
  data_type::type_unsigned_long,
  data_type::type_unsigned_long_long,
  data_type::type_float,
  data_type::type_double,
  data_type::type_bool,
  data_type::type_char_pointer,
  data_type::type_text,
  data_type::type_date,
  data_type::type_time,
  data_type::type_blob,
  data_type::type_unknown,
  data_type::type_null
};

data_type determine_type(const sql::column_type &value, size_t size) {
  const auto type = index2type[value.index()];
  if (type == data_type::type_text && size > 0) {
    return data_type::type_varchar;
  }

  return type;
}

}

value::value()
: token(detail::token::VALUE)
{}

value::value(matador::data_type data_type, size_t size)
: token(detail::token::VALUE)
, size_(size)
, type_(data_type) {}

value::value(sql::column_type value, size_t size)
: token(detail::token::VALUE)
, value_(std::move(value))
, size_(size)
, type_(detail::determine_type(value, size))
{}

value::value(value &&x) noexcept
: token(detail::token::VALUE)
, value_(std::move(x.value_))
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

void value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

std::string value::str() const
{
  return as<std::string>().value();
}

size_t value::size() const
{
  return size_;
}

data_type value::data_type() const
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

bool value::is_blob() const
{
  return type_ == data_type::type_blob;
}

bool value::is_date() const
{
  return type_ == data_type::type_date;
}

bool value::is_time() const
{
  return type_ == data_type::type_time;
}

bool value::is_null() const
{
  return type_ == data_type::type_null;
}

bool value::is_unknown() const
{
  return type_ == data_type::type_unknown;
}

const sql::column_type &value::operator()() const {
  return value_;
}

}
