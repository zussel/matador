#include "json/json_value.hpp"
#include "json/json_object.hpp"
#include "json/json_string.hpp"
#include "json/json_bool.hpp"
#include "json/json_null.hpp"
#include "json/json_number.hpp"
#include "json/json_array.hpp"

namespace oos {

json_value::json_value()
  : type_(new json_null)
{}

json_value::json_value(const json_value &x)
  : type_(x.type_)
{
}

json_value::json_value(json_type *x)
  : type_(x)
{
}

json_value::json_value(const std::string &x)
  : type_(new json_string(x))
{}

json_value::json_value(const char *x)
  : type_(new json_string(x))
{}

json_value::json_value(int x)
  : type_(new json_number(x))
{}

json_value::json_value(double x)
  : type_(new json_number(x))
{}

json_value::json_value(bool x)
  : type_(new json_bool(x))
{}

json_value& json_value::operator=(const json_value &x)
{
  type_ = x.type_;
  return *this;
}

json_value& json_value::operator=(json_type *x)
{
  type_.reset(x);
  return *this;
}

json_value& json_value::operator=(const std::string &x)
{
  type_.reset(new json_string(x));
  return *this;
}

json_value& json_value::operator=(const char *x)
{
  type_.reset(new json_string(x));
  return *this;
}

json_value& json_value::operator=(double x)
{
  type_.reset(new json_number(x));
  return *this;
}

json_value& json_value::operator=(bool x)
{
  type_.reset(new json_bool(x));
  return *this;
}

json_value::~json_value()
{}

json_value& json_value::operator[](const std::string &key)
{
  return type_->operator [](key);
}

json_value& json_value::operator[](size_t index)
{
  return type_->operator [](index);
}

const json_value& json_value::operator[](size_t index) const
{
  return type_->operator [](index);
}

void json_value::push_back(const json_value &x)
{
  type_->push_back(x);
}

size_t json_value::size() const
{
  return type_->size();
}

json_value json_value::create(std::istream &in)
{
  // eat whitespace
  in >> std::ws;

  char c = in.peek();

  if (!in.good()) {
    throw std::logic_error("invalid stream");
  }

  switch (c) {
    case '{':
      return json_value(new json_object);
    case '[':
      return json_value(new json_array);
    case '"':
      return json_value(new json_string);
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return json_value(new json_number);
    case 't':
    case 'f':
      return json_value(new json_bool);
    case 'n':
      return json_value(new json_null);
    default:
      throw std::logic_error("unknown json type");
  }
}

std::istream& operator>>(std::istream &str, json_value &value)
{
  value.type_->parse(str);
  return str;
}

std::ostream& operator<<(std::ostream &str, const json_value &value)
{
  value.type_->print(str);
  return str;
}

}
