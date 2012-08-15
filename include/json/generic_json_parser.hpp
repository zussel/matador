/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GENERIC_JSON_PARSER_HPP
#define GENERIC_JSON_PARSER_HPP

#include <iostream>

namespace oos {

template < class T >
class generic_json_parser
{
private:
  generic_json_parser(const generic_json_parser &);
  generic_json_parser& operator=(const generic_json_parser &);

protected:
  generic_json_parser(T *h) : handler_(h) {}

public:
  virtual ~generic_json_parser() {}

  void parse(std::istream &in);

private:
  void parse_json_object(std::istream &in);
  void parse_json_array(std::istream &in);
  std::string parse_json_string(std::istream &in);
  double parse_json_number(std::istream &in);
  bool parse_json_bool(std::istream &in);
  void parse_json_null(std::istream &in);
  void parse_json_value(std::istream &in);

private:
  T *handler_;

  static const char *null_string;
  static const char *true_string;
  static const char *false_string;
};

template < class T > const char *generic_json_parser<T>::null_string = "null";
template < class T > const char *generic_json_parser<T>::true_string = "true";
template < class T > const char *generic_json_parser<T>::false_string = "false";

template < class T >
void
generic_json_parser<T>::parse(std::istream &in)
{
  // eat whitespace
  in >> std::ws;

  char c = in.peek();

  if (!in.good()) {
    // Todo: throw a json_error instead
    throw std::logic_error("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object(in);
      break;
    case '[':
      parse_json_array(in);
      break;
    default:
      // Todo: throw a json_error instead
      throw std::logic_error("root must be either array '[]' or object '{}'");
  }

  // skip white
  in >> std::ws;

  // no characters after closing parenthesis are aloud
  if (!in.eof()) {
    throw std::logic_error("no characters are allowed after closed root node");
  }
}

template < class T >
void
generic_json_parser<T>::parse_json_object(std::istream &in)
{
  in >> std::ws;

  /*
   * parse '{'
   * 
   * parse string (key)
   * 
   * parse ':'
   * 
   * parse value (object, string, array, number, bool or null)
   * 
   * parse '}'
   */
  char c(0);
  
  c = in.get();
  if (in.good() && c != '{') {
    in.putback(c);
    // Todo: throw json_error
    throw std::logic_error("character isn't object opening bracket");
  }

  /* 
   * call handler callback
   */
  handler_->on_begin_object();

  c = in.get();
  if (in.good() && c == '}') {
    /*
     * call handler callback
     */
    handler_->on_end_object();
    // empty object
    return;
  }
  
  // skip white
  in >> std::ws;

  do {

    /* 
     * call handler callback
     */
    handler_->on_object_key(parse_json_string(in));

    // skip ws
    in >> std::ws;

    // read colon
    c = in.get();
    if (in.good() && c != ':') {
      // Todo: throw json_error
      throw std::logic_error("character isn't colon");
    }

    parse_json_value(in);

    // skip white
    in >> std::ws;

    c = in.get();
  } while (in && in.good() && c == ',');

  if (c != '}') {
    throw std::logic_error("not a valid object closing bracket");
  }

  /*
   * call handler callback
   */
  handler_->on_end_object();
}

template < class T >
void
generic_json_parser<T>::parse_json_array(std::istream &in)
{
  // skip white
  in >> std::ws;

  char c(0);
  
  c = in.get();
  if (in.good() && c != '[') {
    in.putback(c);
    throw std::logic_error("not an array");
  }
  
  handler_->on_begin_array();

  c = in.get();
  if (in.good() && c == ']') {
    handler_->on_end_array();
    // empty array
    return;
  }
  
  // skip white
  in >> std::ws;

  do {

    // skip ws
    in >> std::ws;

    parse_json_value(in);

    // skip white
    in >> std::ws;

    c = in.get();
  } while (in && in.good() && c == ',');

  if (c != ']') {
    throw std::logic_error("not a valid array closing bracket");
  }

  handler_->on_end_array();
}

template < class T >
std::string
generic_json_parser<T>::parse_json_string(std::istream &in)
{
  // skip white
  in >> std::ws;
  
  // parse string
  char c = in.get();
  if (in.good() && c != '"') {
    // invalid key
    in.putback(' ');
    in.putback(c);
    // Todo: throw json_error instead
    throw std::logic_error("invalid json character");
  }

  std::string value;
  // read until next '"' or eof of stream

  while (in && in.good()) {
    c = in.get();
    if (!in.good()) {
      continue;
    }

    if (c == '"') {
      // read closing double quote
      break;
    } else if (c == '\\') {
      c = in.get();
      switch (c) {
        case '"':
        case '\\':
        case '/':
          value.push_back(c);
          break;
        case 'b':
          value.push_back('\b');
          break;
        case 'f':
          value.push_back('\f');
          break;
        case 'n':
          value.push_back('\n');
          break;
        case 'r':
          value.push_back('\r');
          break;
        case 't':
          value.push_back('\t');
          break;
        case 'u':
          // read four more hex digits
          value.push_back('\\');
          value.push_back('u');
          for (int i = 0; i < 4; ++i) {
            c = in.get();
            if (isxdigit(c)) {
              value.push_back(c);
            } else {
              // Todo: throw json_error instead
              throw std::logic_error("invalid json character");
            }
          }
          break;
        default:
          // Todo: throw json_error instead
          throw std::logic_error("invalid json character");
      }
    } else {
      value.push_back(c);
    }
  }
  return value;
}

template < class T >
double
generic_json_parser<T>::parse_json_number(std::istream &in)
{
  // skip white
  in >> std::ws;

  double value;
  in >> value;
  
  char c = in.peek();
  
  switch (c) {
    case ' ':
    case ',':
    case ']':
    case '}':
      break;
    default:
      // Todo: throw json_error instead
      throw std::logic_error("invalid json character");
  }
  return value;
}

template < class T >
bool
generic_json_parser<T>::parse_json_bool(std::istream &in)
{
  // skip white
  in >> std::ws;
  
  // parse string
  char c = in.get();

  if (!in.good()) {
    // Todo: throw json_error instead
    throw std::logic_error("input stream isn't good");
  }

  int i = 1;
  if (c == 't') {
    // check for "true"
    while (in && i < 4) {
      c = in.get();
      if (!in.good()) {
        // Todo: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::true_string[i++] != c) {
        // Todo: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
    return true;

  } else if (c == 'f') {
    // check for "false"
    while (in && i < 5) {
      c = in.get();
      if (!in.good()) {
        // Todo: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::false_string[i++] != c) {
        // Todo: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
    return false;
  } else {
    // Todo: throw json_error instead
    throw std::logic_error("invalid bool character");
  }
}

template < class T >
void
generic_json_parser<T>::parse_json_null(std::istream &in)
{
  // skip white
  in >> std::ws;
  
  // parse string
  char c = in.get();

  if (!in.good()) {
    // Todo: throw json_error instead
    throw std::logic_error("input stream isn't good");
  }

  int i = 1;
  if (c == 'n') {
    // check for "null"
    while (in && i < 4) {
      c = in.get();
      if (!in.good()) {
        // Todo: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::null_string[i++] != c) {
        // Todo: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
  } else {
    // Todo: throw json_error instead
    throw std::logic_error("invalid bool character");
  }
}

template < class T >
void
generic_json_parser<T>::parse_json_value(std::istream &in)
{
  // skip ws
  in >> std::ws;

  // parse value of key
  char c = in.peek();

  if (!in.good()) {
    throw std::logic_error("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object(in);
      break;
    case '[':
      parse_json_array(in);
      break;
    case '"':
      handler_->on_string(parse_json_string(in));
      break;
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
      handler_->on_number(parse_json_number(in));
      break;
    case 't':
    case 'f':
      handler_->on_bool(parse_json_bool(in));
      break;
    case 'n':
      parse_json_null(in);
      handler_->on_null();
      break;
    default:
      throw std::logic_error("unknown json type");
  }
}

}

#endif /* GENERIC_JSON_PARSER_HPP */
