#include "json/json_type.hpp"

#include <stdexcept>

namespace oos {

json_parser json_type::parser = json_parser();

json_type::json_type(const std::string &name)
  : type_(name)
{}

json_type::~json_type()
{}

json_value& json_type::operator[](const std::string &)
{
  throw std::logic_error(type_ + " has no key access operator");
}

json_value& json_type::operator[](size_t )
{
  throw std::logic_error(type_ + " has no index access operator");
}

const json_value& json_type::operator[](size_t ) const
{
  throw std::logic_error(type_ + " has no index access operator");
}

void json_type::push_back(const json_value &)
{
  throw std::logic_error(type_ + " has no push_back method");
}

std::string json_type::type() const
{
  return type_;
}

void json_type::type(const std::string &type)
{
  type_ = type;
}

size_t json_type::size() const
{
  throw std::logic_error(type_ + " has no size method");
}

/*
std::istream& operator>>(std::istream &str, json_type &value)
{
  value.parse(str);
  return str;
}
*/

std::ostream& operator<<(std::ostream &str, const json_type &value)
{
  value.print(str);
  return str;
}

}
