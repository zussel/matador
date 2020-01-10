//
// Created by sascha on 09.01.20.
//

#ifndef MATADOR_GENERIC_JSON_PARSER_NG_HPP
#define MATADOR_GENERIC_JSON_PARSER_NG_HPP

#include "matador/utils/string.hpp"
#include "matador/utils/json_exception.hpp"

#include <string>

namespace matador {

template < class T >
class generic_json_parser_ng
{
protected:
  /**
   * Creates a new generic_json_parser for
   * concrete parser h.
   *
   * @param h The concrete parser.
   */
  explicit generic_json_parser_ng(T *h) : handler_(h) {}

  struct real_t {
    double real = 0.0;
    long long integer = 0;
    bool is_real = false;
  };

public:
  virtual ~generic_json_parser_ng() = default;

protected:
  void parse_json(const char *json_str);

private:
  void parse_json_object();
  void parse_json_array();
  std::string parse_json_string();
  real_t parse_json_number();
  bool parse_json_bool();
  void parse_json_null();
  void parse_json_value();

  char skip_whitespace();
  char next_char();

private:
  T *handler_;

  static const char *null_string;
  static const char *true_string;
  static const char *false_string;

  const char *json_cursor = nullptr;
};

template < class T > const char *generic_json_parser_ng<T>::null_string = "null";
template < class T > const char *generic_json_parser_ng<T>::true_string = "true";
template < class T > const char *generic_json_parser_ng<T>::false_string = "false";

template < class T >
void
generic_json_parser_ng<T>::parse_json(const char *json_str)
{
  json_cursor = json_str;

  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object();
      break;
    case '[':
      parse_json_array();
      break;
    default:
      throw json_exception("root must be either array '[]' or serializable '{}'");
  }

  // skip white
  c = skip_whitespace();

  // no characters after closing parenthesis are aloud
  if (!is_eos(c)) {
    throw json_exception("no characters are allowed after closed root node");
  }
}

template < class T >
void
generic_json_parser_ng<T>::parse_json_object()
{
  char c = skip_whitespace();

  /*
   * parse '{'
   * parse string (key)
   * parse ':'
   * parse value (serializable, string, array, number, bool or null)
   * parse '}'
   */
  if (!is_eos(c) && c != '{') {
    throw json_exception("character isn't serializable opening bracket");
  }

  c = next_char();

  // call handler callback
  handler_->on_begin_object();

  if (!is_eos(c) && c == '}') {
    next_char();
    // call handler callback
    handler_->on_end_object();
    return;
  }

  skip_whitespace();

  bool has_next = false;
  do {

    // get key and call handler callback
    handler_->on_object_key(parse_json_string());

    c = skip_whitespace();
    // read colon
    if (!is_eos(c) && c != ':') {
      throw json_exception("character isn't colon");
    }

    next_char();

    parse_json_value();

    c = skip_whitespace();

    if (c == ',') {
      has_next = true;
      next_char();
    } else {
      has_next = false;
    }
  } while (!is_eos(c) && has_next);

  if (!is_eos(c) && c != '}') {
    throw json_exception("not a valid serializable closing bracket");
  }
  next_char();

  // call handler callback
  handler_->on_end_object();
}

template<class T>
void generic_json_parser_ng<T>::parse_json_array()
{
  char c = skip_whitespace();

  if (!is_eos(c) && c != '[') {
    throw json_exception("not an array");
  }

  handler_->on_begin_array();

  c = next_char();
  if (!is_eos(c) && c == ']') {
    handler_->on_end_array();
    // empty array
    return;
  }

  skip_whitespace();

  bool has_next = false;
  do {

    c = skip_whitespace();

    parse_json_value();

    c = skip_whitespace();

    if (c == ',') {
      has_next = true;
      next_char();
    } else {
      has_next = false;
    }
  } while (!is_eos(c) && has_next);

  if (c != ']') {
    throw json_exception("not a valid array closing bracket");
  }
  next_char();

  handler_->on_end_array();
}

template<class T>
std::string generic_json_parser_ng<T>::parse_json_string()
{
  char c = skip_whitespace();

  // parse string
  if (!is_eos(c) && c != '"') {
    throw json_exception("invalid json character");
  }
  
  std::string value;
  // read until next '"' or eof of stream
  c = next_char();
  while (!is_eos(c)) {

    if (c == '"') {
      // read closing double quote
      next_char();
      break;
    } else if (c == '\\') {
      c = next_char();
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
            c = next_char();
            if (isxdigit(c)) {
              value.push_back(c);
            } else {
              throw json_exception("invalid json character");
            }
          }
          break;
        default:
          throw json_exception("invalid json character");
      }
    } else {
      value.push_back(c);
    }
    c = next_char();
  }
  return value;
}

template<class T>
typename generic_json_parser_ng<T>::real_t generic_json_parser_ng<T>::parse_json_number()
{
  char c = skip_whitespace();

  // number could be an integer
  // or a floating point
  real_t value;
  char *end;
  value.integer = strtoll(json_cursor, &end, 10);
  if (errno == ERANGE) {
    throw json_exception("errno integer error");
  }

  if (!is_eos(end[0]) && end[0] == '.') {
    // value is double
    value.is_real = true;
    value.integer = 0;
    value.real = strtod(json_cursor, &end);
    if (errno == ERANGE) {
      throw json_exception("errno double error");
    }
  }

  json_cursor = end;

  c = end[0];

  switch (c) {
    case ' ':
    case ',':
    case ']':
    case '}':
      break;
    default:
      throw json_exception("invalid json character");
  }
  return value;
}

template<class T>
bool generic_json_parser_ng<T>::parse_json_bool()
{
  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("input stream isn't good");
  }

  int i = 1;
  if (c == 't') {
    // check for "true"
    while (i < 4) {
      c = next_char();
      if (is_eos(c)) {
        throw json_exception("input stream isn't good");
      }
      if (generic_json_parser_ng<T>::true_string[i++] != c) {
        throw json_exception("invalid bool character");
      }
    }
    next_char();
    return true;

  } else if (c == 'f') {
    // check for "false"
    while (i < 5) {
      c = next_char();
      if (is_eos(c)) {
        throw json_exception("input stream isn't good");
      }
      if (generic_json_parser_ng<T>::false_string[i++] != c) {
        throw json_exception("invalid bool character");
      }
    }
    next_char();
    return false;
  } else {
    throw json_exception("invalid bool character");
  }
}

template<class T>
void generic_json_parser_ng<T>::parse_json_null()
{
  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("input stream isn't good");
  }

  int i = 1;
  if (c == 'n') {
    // check for "null"
    while (i < 4) {
      c = next_char();
      if (is_eos(c)) {
        throw json_exception("input stream isn't good");
      }
      if (generic_json_parser_ng<T>::null_string[i++] != c) {
        throw json_exception("invalid bool character");
      }
    }
    next_char();
  } else {
    throw std::logic_error("invalid bool character");
  }
}

template<class T>
void generic_json_parser_ng<T>::parse_json_value()
{
  // parse value of key
  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object();
      break;
    case '[':
      parse_json_array();
      break;
    case '"':
      handler_->on_string(parse_json_string());
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
    case '.':
      handler_->on_number(parse_json_number());
      break;
    case 't':
    case 'f':
      handler_->on_bool(parse_json_bool());
      break;
    case 'n':
      parse_json_null();
      handler_->on_null();
      break;
    default:
      throw json_exception("unknown json type");
  }

}

template<class T>
char generic_json_parser_ng<T>::skip_whitespace()
{
  json_cursor = skip_ws(json_cursor);
  return json_cursor[0];
}

template<class T>
char generic_json_parser_ng<T>::next_char()
{
  if (json_cursor == nullptr) {
    throw json_exception("no current json string");
  }
  return (++json_cursor)[0];
}

}

#endif //MATADOR_GENERIC_JSON_PARSER_NG_HPP
