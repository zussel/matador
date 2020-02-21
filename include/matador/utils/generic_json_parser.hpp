#ifndef MATADOR_GENERIC_JSON_PARSER_HPP
#define MATADOR_GENERIC_JSON_PARSER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "matador/utils/string.hpp"
#include "matador/utils/json_exception.hpp"

#include <string>
#include <cstring>
#include <climits>
#include <cmath>
#include <iostream>

namespace matador {

template < class T >
class generic_json_parser
{
protected:
  /**
   * Creates a new generic_json_parser for
   * concrete parser h.
   *
   * @param h The concrete parser.
   */
  generic_json_parser() = default;

public:
  struct number_t {
    double real = 0.0;
    long long integer = 0;
    bool is_real = false;
  };

  const char* json_cursor() const;

public:
  virtual ~generic_json_parser() = default;

protected:
  void on_parse_object(bool check_for_eos);
  void on_parse_array(bool check_for_eos);
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_number(number_t value);
  void on_bool(bool value);
  void on_null();

protected:
  void parse_json(const char *json_str, bool check_for_eos = true);
  void parse_json_object(const char *json_str, bool check_for_eos = true);
  void parse_json_array(const char *json_str, bool check_for_eos = true);

  void sync_cursor(const char *cursor);

private:
  void parse_json_object(bool check_for_eos);
  void parse_json_array(bool check_for_eos);
  std::string parse_json_string();
  number_t parse_json_number();
  bool parse_json_bool();
  void parse_json_null();
  void parse_json_value();

  char skip_whitespace();
  char next_char();
  char current_char() const;

  void compare_string(const char *to_compare, size_t len);

private:
  static const char *null_string;
  static const char *true_string;
  static const char *false_string;

  const char *json_cursor_ = nullptr;
};

template < class T > const char *generic_json_parser<T>::null_string = "null";
template < class T > const char *generic_json_parser<T>::true_string = "true";
template < class T > const char *generic_json_parser<T>::false_string = "false";

template<class T>
void generic_json_parser<T>::on_begin_object()
{

}

template<class T>
void generic_json_parser<T>::on_object_key(const std::string &key)
{

}

template<class T>
void generic_json_parser<T>::on_end_object()
{

}

template<class T>
void generic_json_parser<T>::on_begin_array()
{

}

template<class T>
void generic_json_parser<T>::on_end_array()
{

}

template<class T>
void generic_json_parser<T>::on_string(const std::string &value)
{

}

template<class T>
void generic_json_parser<T>::on_number(number_t value)
{

}

template<class T>
void generic_json_parser<T>::on_bool(bool value)
{

}

template<class T>
void generic_json_parser<T>::on_null()
{

}

template < class T >
void
generic_json_parser<T>::parse_json(const char *json_str, bool check_for_eos)
{
  json_cursor_ = json_str;

  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object(check_for_eos);
      break;
    case '[':
      parse_json_array(check_for_eos);
      break;
    default:
      throw json_exception("root must be either array '[]' or object '{}'");
  }

  // skip white
  c = skip_whitespace();

  // no characters after closing parenthesis are allowed
  if (check_for_eos && !is_eos(c)) {
    throw json_exception("no characters are allowed after closed root node");
  }
}

template<class T>
void generic_json_parser<T>::parse_json_object(const char *json_str, bool check_for_eos)
{
  json_cursor_ = json_str;

  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  if (c != '{') {
    throw json_exception("root must be object '{}'");
  }

  parse_json_object(check_for_eos);

  // skip white
  c = skip_whitespace();

  // no characters after closing parenthesis are allowed
  if (check_for_eos && !is_eos(c)) {
    throw json_exception("no characters are allowed after closed root node");
  }
}

template<class T>
void generic_json_parser<T>::parse_json_array(const char *json_str, bool check_for_eos)
{
  json_cursor_ = json_str;

  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  if (c != '[') {
    throw json_exception("root must be array '[]'");
  }

  parse_json_array(check_for_eos);

  // skip white
  c = skip_whitespace();

  // no characters after closing parenthesis are allowed
  if (check_for_eos && !is_eos(c)) {
    throw json_exception("no characters are allowed after closed root node");
  }
}

template<class T>
const char *generic_json_parser<T>::json_cursor() const
{
  return json_cursor_;
}

template<class T>
void generic_json_parser<T>::sync_cursor(const char *cursor)
{
  json_cursor_ = cursor;
}

template < class T >
void
generic_json_parser<T>::parse_json_object(bool check_for_eos)
{
  T& t = static_cast<T&>(*this);
  t.on_parse_object(check_for_eos);
}

template < class T >
void
generic_json_parser<T>::on_parse_object(bool check_for_eos)
{
//  std::cout << "(this: " << this << ") start parse_json_object, cursor: [" << json_cursor_ << "]\n";

  /*
   * parse '{'
   * parse string (key)
   * parse ':'
   * parse value (serializable, string, array, number, bool or null)
   * parse '}'
   */
  // call handler callback
  static_cast<T*>(this)->on_begin_object();

  next_char();

  char c = skip_whitespace();

  if (!is_eos(c) && c == '}') {
    next_char();
    // call handler callback
    static_cast<T*>(this)->on_end_object();
    return;
  }

  bool has_next;

  do {

    // get key and call handler callback
    static_cast<T*>(this)->on_object_key(parse_json_string());

    c = skip_whitespace();
    // read colon
    if (is_eos(c)) {
      throw json_exception("invalid stream");
    } else if (c != ':') {
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

  c = skip_whitespace();

  if (!check_for_eos && is_eos(c)) {
    throw json_exception("unexpected end of string");
  } else if (c != '}') {
    throw json_exception("not a valid object closing bracket");
  }

  static_cast<T*>(this)->on_end_object();

  if (check_for_eos) {
    next_char();
  }
}

template < class T >
void
generic_json_parser<T>::parse_json_array(bool check_for_eos)
{
  T& t = static_cast<T&>(*this);
  t.on_parse_array(check_for_eos);
}

template<class T>
void generic_json_parser<T>::on_parse_array(bool check_for_eos)
{
  static_cast<T*>(this)->on_begin_array();

  next_char();
  char c = skip_whitespace();
  if (!is_eos(c) && c == ']') {
    next_char();
    static_cast<T*>(this)->on_end_array();
    // empty array
    return;
  }

  skip_whitespace();

  bool has_next;
  do {

    parse_json_value();

    c = skip_whitespace();

    if (c == ',') {
      has_next = true;
      next_char();
    } else {
      has_next = false;
    }
  } while (!is_eos(c) && has_next);

  c = skip_whitespace();

  if (!check_for_eos && is_eos(c)) {
    throw json_exception("unexpected end of string");
  } else if (c != ']') {
    throw json_exception("not a valid array closing bracket");
  }

  static_cast<T*>(this)->on_end_array();

  if (check_for_eos) {
    next_char();
  }
}

template<class T>
std::string generic_json_parser<T>::parse_json_string()
{
  char c = skip_whitespace();

  // parse string
  if (!is_eos(c) && c != '"') {
    throw json_exception("expected string opening quotes");
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
              throw json_exception("invalid json string hex character");
            }
          }
          break;
        default:
          throw json_exception("invalid json control string character");
      }
    } else {
      value.push_back(c);
    }
    c = next_char();
  }
  return value;
}

OOS_UTILS_API bool is_error(const char *start, const char *end, long long value);
OOS_UTILS_API bool is_error(const char *start, const char *end, double value);

template<class T>
typename generic_json_parser<T>::number_t generic_json_parser<T>::parse_json_number()
{
  // number could be an integer
  // or a floating point
  number_t value;
  char *end;
  value.integer = strtoll(json_cursor_, &end, 10);
  if (is_error(json_cursor_, end, value.integer)) {
    throw json_exception("errno integer error");
  }

  if (!is_eos(end[0]) && end[0] == '.') {
    // value is double
    value.is_real = true;
    value.integer = 0;
    value.real = strtod(json_cursor_, &end);
    if (is_error(json_cursor_, end, value.real)) {
      throw json_exception("errno double error");
    }
  }

  json_cursor_ = end;

  char c = end[0];

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
bool generic_json_parser<T>::parse_json_bool()
{
  char c = current_char();

  if (c == 't') {
    // check for "true"
    compare_string(generic_json_parser<T>::true_string, 4);
    next_char();
    return true;
  } else if (c == 'f') {
    // check for "false"
    compare_string(generic_json_parser<T>::false_string, 5);
    next_char();
    return false;
  } else {
    throw json_exception("invalid bool character");
  }
}

template<class T>
void generic_json_parser<T>::compare_string(const char *to_compare, size_t len)
{
  size_t i{1};
  char c{};
  while (i < len) {
    c = next_char();
    if (is_eos(c)) {
      throw json_exception("unexpected end of string");
    }
    if (to_compare[i++] != c) {
      throw json_exception("invalid character for bool value string");
    }
  }
}

template<class T>
void generic_json_parser<T>::parse_json_null()
{
  char c = current_char();

  int i = 1;
  if (c == 'n') {
    // check for "null"
    while (i < 4) {
      c = next_char();
      if (is_eos(c)) {
        throw json_exception("unexpected end of string");
      }
      if (generic_json_parser<T>::null_string[i++] != c) {
        throw json_exception("invalid null character");
      }
    }
    next_char();
  } else {
    throw std::logic_error("invalid bool character");
  }
}

template<class T>
void generic_json_parser<T>::parse_json_value()
{
  // parse value of key
  char c = skip_whitespace();

  if (is_eos(c)) {
    throw json_exception("invalid stream");
  }

  switch (c) {
    case '{':
      parse_json_object(false);
      break;
    case '[':
      parse_json_array(true);
      break;
    case '"':
      static_cast<T*>(this)->on_string(parse_json_string());
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
      static_cast<T*>(this)->on_number(parse_json_number());
      break;
    case 't':
    case 'f':
      static_cast<T*>(this)->on_bool(parse_json_bool());
      break;
    case 'n':
      parse_json_null();
      static_cast<T*>(this)->on_null();
      break;
    default:
      throw json_exception("unknown json type");
  }

}

template<class T>
char generic_json_parser<T>::skip_whitespace()
{
  json_cursor_ = skip_ws(json_cursor_);
  return json_cursor_[0];
}

template<class T>
char generic_json_parser<T>::next_char()
{
  if (json_cursor_ == nullptr) {
    throw json_exception("no current json string");
  }
  return (++json_cursor_)[0];
}

template<class T>
char generic_json_parser<T>::current_char() const
{
  return json_cursor_[0];
}

}

#endif //MATADOR_GENERIC_JSON_PARSER_HPP
