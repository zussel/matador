#include "json/json_bool.hpp"

namespace oos {

const char *json_bool::true_string = "true";
const char *json_bool::false_string = "false";

json_bool::json_bool(void)
  : value_(true)
{}

json_bool::json_bool(bool val)
  : value_(val)
{}

json_bool::~json_bool(void)
{}

bool json_bool::parse(std::istream &in)
{
  // skip white
  in >> std::ws;
  
  // parse string
  char c = in.get();

  if (!in.good()) {
    return false;
  }

  int i = 0;
  if (c == 't') {
    // check for "true"
    while (in && i < 4) {
      if (!in.good()) {
        return false;
      }
      if (json_bool::true_string[i++] != c) {
        return false;
      }
      c = in.get();
    }
    value_ = true;

  } else if (c == 'f') {
    // check for "true"
    // check for "true"
    while (in && i < 5) {
      if (!in.good()) {
        return false;
      }
      if (json_bool::false_string[i++] != c) {
        return false;
      }
      c = in.get();
    }
    value_ = false;
  }
  return true;
}

void json_bool::print(std::ostream &out) const
{
  out << (value_ ? json_bool::true_string : json_bool::false_string);
}

bool json_bool::value() const
{
  return value_;
}

void json_bool::value(bool val)
{
  value_ = val;
}

}