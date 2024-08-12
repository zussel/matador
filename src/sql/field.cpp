#include "matador/sql/field.hpp"

#include <ostream>

namespace matador::sql {

field::field(std::string name)
  : name_(std::move(name))
  , value_(nullptr)
{}

field::field(std::string name, data_type dt, size_t size, int index)
  : name_(std::move(name))
  , index_(index)
  , value_(dt, size) {}

field::field(field &&x) noexcept
  : name_(std::move(x.name_))
  , index_(x.index_)
  , value_(std::move(x.value_))
{
  x.value_ = nullptr;
  x.index_ = -1;
}

field &field::operator=(field &&x) noexcept
{
  name_ = std::move(x.name_);
  index_ = x.index_;
  value_ = std::move(x.value_);
  x.index_ = -1;
  x.value_ = nullptr;

  return *this;
}

const std::string &field::name() const
{
  return name_;
}

size_t field::size() const
{
  return value_.size();
}

int field::index() const
{
  return index_;
}

std::ostream &operator<<(std::ostream &out, const field &col)
{
  out << col.str();
  return out;
}

std::string field::str() const
{
  return as<std::string>().value();
}

bool field::is_integer() const
{
  return value_.is_integer();
}

bool field::is_floating_point() const
{
  return value_.is_floating_point();
}

bool field::is_bool() const
{
  return value_.is_bool();
}

bool field::is_string() const
{
  return value_.is_string();
}

bool field::is_varchar() const
{
  return value_.is_varchar();
}

bool field::is_blob() const
{
  return value_.is_blob();
}

bool field::is_null() const
{
  return value_.is_null();
}

bool field::is_unknown() const
{
  return value_.is_unknown();
}

}