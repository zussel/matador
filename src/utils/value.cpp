#include "matador/utils/value.hpp"

#include <cstring>

namespace matador::utils {
namespace detail {
size_t determine_size(const std::string &val) {
  return val.size();
}

size_t determine_size(const char *val) {
  return strlen(val);
}

size_t determine_size(const blob_type_t &val) {
  return val.size();
}
}

value::value(const basic_type data_type, const size_t size)
: size_(size)
, type_(data_type) {
  initialize_by_basic_type(type_, value_);
}

value::value(value &&x) noexcept
: value_(std::move(x.value_))
  , type_(x.type_) {
  x.value_ = nullptr;
  x.type_ = basic_type::Null;
}

value &value::operator=(value &&x) noexcept {
  value_ = std::move(x.value_);
  type_ = x.type_;
  x.value_ = nullptr;
  x.type_ = basic_type::Null;

  return *this;
}

bool value::operator==(const value &rhs) const {
  return type_ == rhs.type_ && value_ == rhs.value_;
}

bool value::operator!=(const value &rhs) const {
  return !operator==(rhs);
}

std::string value::str() const {
  return as<std::string>().value_or("");
}

size_t value::size() const {
  return size_;
}

basic_type value::type() const {
  return type_;
}

void value::type(const basic_type t) {
  type_ = t;
  initialize_by_basic_type(type_, value_);
}

bool value::is_integer() const {
  return type_ >= basic_type::Int8 && type_ <= basic_type::UInt64;
}

bool value::is_floating_point() const {
  return type_ == basic_type::Float || type_ == basic_type::Double;
}

bool value::is_bool() const {
  return type_ == basic_type::Boolean;
}

bool value::is_string() const {
  return type_ == basic_type::Text;
}

bool value::is_varchar() const {
  return type_ == basic_type::Varchar;
}

bool value::is_date() const {
  return type_ == basic_type::Date;
}

bool value::is_time() const {
  return type_ == basic_type::Time;
}

bool value::is_timestamp() const {
  return type_ == basic_type::DateTime;
}

bool value::is_blob() const {
  return type_ == basic_type::Blob;
}

bool value::is_null() const {
  return type_ == basic_type::Null;
}
const database_type &value::raw_value() const {
  return value_;
}
}  // namespace matador::utils
