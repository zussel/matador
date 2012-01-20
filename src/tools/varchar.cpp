#include "tools/varchar.hpp"

namespace oos {

varchar::varchar(size_type capacity)
  : capacity_(capacity)
{}

varchar::varchar(const varchar &x)
  : capacity_(x.capacity_)
  , data_(x.data_)
{}

varchar& varchar::operator=(const varchar &x)
{
  capacity_ = x.capacity_;
  data_ = x.data_;
  return *this;
}

varchar& varchar::operator=(const std::string &x)
{
  ok(x);
  data_ = x;
  return *this;
}

varchar& varchar::operator=(const char *x)
{
  ok(x);
  data_.assign(x);
  return *this;
}

varchar::~varchar()
{}

varchar& varchar::operator+=(const varchar &x)
{
  data_ += x.str();
  return *this;
}

varchar& varchar::operator+=(const std::string &x)
{
  data_ += x;
  return *this;
}

varchar& varchar::operator+=(const char *x)
{
  data_ += x;
  return *this;
}

operator varchar::std::string() const
{
  return data_;
}

std::string varchar::str() const
{
  return data_;
}

varchar::size_type varchar::size() const
{
  return data_.size();
}

varchar::size_type varchar::capacity() const
{
  return capacity_;
}

friend std::ostream& operator<<(std::ostream &out, const varchar &val)
{
  out << val.str();
  return out;
}

void varchar::ok(const std::string &x)
{
  if (x.size() > capacity_) {
    throw std::logic_error("string is to long");
  }
}

}
