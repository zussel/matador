#include "json/json_bool.hpp"

namespace oos {

const char *json_bool::true_string = "true";
const char *json_bool::false_string = "false";

json_bool::json_bool(void)
  : json_type("json_bool")
  , value_(true)
{}

json_bool::json_bool(bool val)
  : json_type("json_bool")
  , value_(val)
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

  int i = 1;
  if (c == 't') {
    // check for "true"
    while (in && i < 4) {
      c = in.get();
      if (!in.good()) {
        return false;
      }
      if (json_bool::true_string[i++] != c) {
        return false;
      }
    }
    value_ = true;

  } else if (c == 'f') {
    // check for "false"
    while (in && i < 5) {
      c = in.get();
      if (!in.good()) {
        return false;
      }
      if (json_bool::false_string[i++] != c) {
        return false;
      }
    }
    // TODO: better while loop
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
