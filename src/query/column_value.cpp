#include "matador/query/column_value.hpp"

#include <cstring>

namespace matador::query {
namespace detail {
size_t determine_size(const std::string &val) {
  return val.size();
}

size_t determine_size(const char *val) {
  return strlen(val);
}

size_t determine_size(const utils::blob_type_t &val) {
  return val.size();
}
}

column_value::column_value(const utils::basic_type data_type, const size_t size)
: size_(size)
, type_(data_type) {
  initialize_by_basic_type(type_, value_);
}

column_value::column_value(column_value &&x) noexcept
: value_(std::move(x.value_))
  , type_(x.type_) {
  x.value_ = nullptr;
  x.type_ = utils::basic_type::Null;
}

column_value &column_value::operator=(column_value &&x) noexcept {
  value_ = std::move(x.value_);
  type_ = x.type_;
  x.value_ = nullptr;
  x.type_ = utils::basic_type::Null;

  return *this;
}

bool column_value::operator==(const column_value &rhs) const {
  return type_ == rhs.type_ && value_ == rhs.value_;
}

bool column_value::operator!=(const column_value &rhs) const {
  return !operator==(rhs);
}

std::string column_value::str() const {
  return as<std::string>().value_or("");
}

size_t column_value::size() const {
  return size_;
}

utils::basic_type column_value::type() const {
  return type_;
}

void column_value::type(const utils::basic_type t) {
  type_ = t;
  initialize_by_basic_type(type_, value_);
}

bool column_value::is_integer() const {
  return type_ >= utils::basic_type::Int8 && type_ <= utils::basic_type::UInt64;
}

bool column_value::is_floating_point() const {
  return type_ == utils::basic_type::Float || type_ == utils::basic_type::Double;
}

bool column_value::is_bool() const {
  return type_ == utils::basic_type::Boolean;
}

bool column_value::is_string() const {
  return type_ == utils::basic_type::Text;
}

bool column_value::is_varchar() const {
  return type_ == utils::basic_type::Varchar;
}

bool column_value::is_date() const {
  return type_ == utils::basic_type::Date;
}

bool column_value::is_time() const {
  return type_ == utils::basic_type::Time;
}

bool column_value::is_timestamp() const {
  return type_ == utils::basic_type::DateTime;
}

bool column_value::is_blob() const {
  return type_ == utils::basic_type::Blob;
}

bool column_value::is_null() const {
  return type_ == utils::basic_type::Null;
}
const utils::database_type &column_value::raw_value() const {
  return value_;
}
}  // namespace matador::utils
