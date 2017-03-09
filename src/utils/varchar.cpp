#include "matador/utils/varchar.hpp"

#include <algorithm>

namespace matador {

varchar_base::varchar_base(size_type capacity)
  : capacity_(capacity)
{}

varchar_base::varchar_base(const std::string &x, varchar_base::size_type capacity)
  : capacity_(capacity)
  , data_(x)
{}

varchar_base::varchar_base(const char *x, varchar_base::size_type capacity)
  : capacity_(capacity)
  , data_(x)
{}

varchar_base::varchar_base(const varchar_base &x)
  : capacity_(x.capacity_)
  , data_(x.data_)
{}

varchar_base::varchar_base(varchar_base &&x)
  : capacity_(x.capacity_)
  , data_(std::move(x.data_))
{
  x.capacity_ = 0;
}

varchar_base& varchar_base::operator=(const varchar_base &x)
{
  capacity_ = x.capacity_;
  data_ = x.data_;
  return *this;
}

varchar_base &varchar_base::operator=(varchar_base &&x)
{
  data_ = std::move(x.data_);
  capacity_ = x.capacity_;
  x.capacity_ = 0;
  return *this;
}

varchar_base::~varchar_base()
{}

bool varchar_base::operator==(const varchar_base &x) const
{
  return data_ == x.data_;
}

bool varchar_base::operator!=(const varchar_base &x) const
{
  return !this->operator ==(x);
}

varchar_base& varchar_base::operator+=(const varchar_base &x)
{
  data_ += x.str();
  trim();
  return *this;
}

varchar_base& varchar_base::operator+=(const std::string &x)
{
  data_ += x;
  trim();
  return *this;
}

varchar_base& varchar_base::operator+=(const char *x)
{
  data_ += x;
  trim();
  return *this;
}

void varchar_base::assign(const std::string &x)
{
  data_ = x;
}

void varchar_base::assign(const char *s, size_t n)
{
  data_.assign(s, n);
}

void varchar_base::assign(const char *s)
{
  data_.assign(s);
}

std::string varchar_base::str() const
{
  return data_;
}

const char* varchar_base::c_str() const
{
	return data_.c_str();
}

const char* varchar_base::data() const
{
	return data_.data();
}

varchar_base::size_type varchar_base::size() const
{
  return data_.size();
}

varchar_base::size_type varchar_base::capacity() const
{
  return capacity_;
}

std::ostream& operator<<(std::ostream &out, const varchar_base &val)
{
  out << val.str();
  return out;
}

bool varchar_base::valid()
{
  return data_.size() <= capacity_;
}

void varchar_base::trim()
{
  data_.resize(std::min(capacity_, data_.size()));
}

}
