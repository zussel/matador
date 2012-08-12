#include "json/json_number.hpp"

#include <stdexcept>

namespace oos {

json_number::json_number()
{}

json_number::json_number(double val)
  : value_(val)
{}

json_number::json_number(const json_number &x)
  : value_(x.value_)
{}

json_number::json_number(const json_value *x)
{
  const json_number *n = dynamic_cast<const json_number*>(x);
  if (!n) {
    throw std::logic_error("json_value isn't of type json_number");
  } else {
    value_ = n->value_;
  }
}

json_number& json_number::operator=(const json_number &x)
{
  value_ = x.value_;
  return *this;
}

json_number& json_number::operator=(double x)
{
  value_ = x;
  return *this;
}

json_number& json_number::operator=(json_value *x)
{
  const json_number *n = dynamic_cast<const json_number*>(x);
  
  if (n) {
  }
  return *this;
}

json_number::~json_number()
{}

bool json_number::operator<(const json_number &x) const
{
  return value_ < x.value_;
}

bool json_number::parse(std::istream &in)
{
  // skip white
  in >> std::ws;
  
  in >> value_;
  
  char c = in.peek();
  
  switch (c) {
    case ' ':
    case ',':
    case ']':
    case '}':
      break;
    default:
      return false;
  }
  return true;
}

void json_number::print(std::ostream &out) const
{
  out << value_;
}

double json_number::value() const
{
  return value_;
}

void json_number::value(double val)
{
  value_ = val;
}

}
