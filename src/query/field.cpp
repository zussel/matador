#include "matador/query/field.hpp"

namespace matador::query {
field::field(std::string name)
  : name_(std::move(name))
  , value_(nullptr)
{}

field::field(std::string name, const utils::basic_type dt, const column_constraint type, const size_t size, const int index)
: name_(std::move(name))
, type_(type)
, index_(index)
, value_(dt, size) {}

field::field(field &&x) noexcept
: name_(std::move(x.name_))
, type_(x.type_)
, index_(x.index_)
, value_(std::move(x.value_)) {
  x.value_ = nullptr;
  x.index_ = -1;
}

field &field::operator=(field &&x) noexcept {
  name_ = std::move(x.name_);
  type_ = x.type_;
  index_ = x.index_;
  value_ = std::move(x.value_);
  x.index_ = -1;
  x.value_ = nullptr;

  return *this;
}

bool field::operator==(const field &rhs) const {
  return name_ == rhs.name_ &&
    type_ == rhs.type_ &&
    index_ == rhs.index_ &&
    value_ == rhs.value_;
}

bool field::operator!=(const field &rhs) const {
  return !operator==(rhs);
}

const std::string &field::name() const {
  return name_;
}

column_constraint field::type() const {
  return type_;
}

size_t field::size() const {
  return value_.size();
}

int field::index() const {
  return index_;
}

column_value field::value() const {
  return value_;
}

std::ostream &operator<<(std::ostream &out, const field &col) {
  out << col.str();
  return out;
}

std::string field::str() const {
  return as<std::string>().value_or("");
}

bool field::is_integer() const {
  return value_.is_integer();
}

bool field::is_floating_point() const {
  return value_.is_floating_point();
}

bool field::is_bool() const {
  return value_.is_bool();
}

bool field::is_string() const {
  return value_.is_string();
}

bool field::is_varchar() const {
  return value_.is_varchar();
}

bool field::is_blob() const {
  return value_.is_blob();
}

bool field::is_null() const {
  return value_.is_null();
}

bool field::is_primary_key() const {
  return type_ == column_constraint::PrimaryKey;
}

bool field::is_foreign_key() const {
  return type_ == column_constraint::ForeignKey;
}

bool field::is_attribute() const {
  return !is_primary_key() && !is_foreign_key();
}


}