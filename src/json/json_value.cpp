#include "json/json_value.hpp"
#include "json/json_object.hpp"
#include "json/json_string.hpp"
#include "json/json_bool.hpp"
#include "json/json_null.hpp"
#include "json/json_array.hpp"

#include <iostream>

namespace oos {

json_value* json_value::create(std::istream &in)
{
  // eat whitespace
  in >> std::ws;

  char c = in.peek();

  if (!in.good()) {
    return 0;
  }

  switch (c) {
    case '{':
      return new json_object;
    case '[':
      return new json_array;
    case '"':
      return new json_string;
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
      //return new json_number;
      return 0;
    case 't':
    case 'f':
      return new json_bool;
    case 'n':
      return new json_null;
  }
  return 0;
}

std::istream& operator>>(std::istream &str, json_value &value)
{
  value.parse(str);
  return str;
}

std::ostream& operator<<(std::ostream &str, const json_value &value)
{
  value.print(str);
  return str;
}

}