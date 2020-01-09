//
// Created by sascha on 09.01.20.
//

#ifndef MATADOR_GENERIC_JSON_PARSER_NG_HPP
#define MATADOR_GENERIC_JSON_PARSER_NG_HPP

#include <string>
#include <stdexcept>

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
  void parse_json_object(const char *json_str);
  void parse_json_array(const char *json_str);
  std::string parse_json_string(const char *json_str);
  real_t parse_json_number(const char *json_str);
  bool parse_json_bool(const char *json_str);
  void parse_json_null(const char *json_str);
  void parse_json_value(const char *json_str);

  const char* skip_whitespace(const char *str);
  bool is_end_of_string(char c);
private:
  T *handler_;

  static const char *null_string;
  static const char *true_string;
  static const char *false_string;
};

template < class T > const char *generic_json_parser_ng<T>::null_string = "null";
template < class T > const char *generic_json_parser_ng<T>::true_string = "true";
template < class T > const char *generic_json_parser_ng<T>::false_string = "false";

template < class T >
void
generic_json_parser_ng<T>::parse_json(const char *json_str)
{
  const char *str = skip_whitespace(json_str);

  char c = str[0];

//  if (!in.good()) {
//    // TODO: throw json_error instead
//    throw std::logic_error("invalid stream");
//  }

  switch (c) {
    case '{':
      parse_json_object(str);
      break;
    case '[':
      parse_json_array(str);
      break;
    default:
      // TODO: throw json_error instead
      throw std::logic_error("root must be either array '[]' or serializable '{}'");
  }

  // skip white
  in >> std::ws;

  // no characters after closing parenthesis are aloud
  if (!in.eof()) {
    // TODO: throw json_error instead
    throw std::logic_error("no characters are allowed after closed root node");
  }
}

template < class T >
void
generic_json_parser_ng<T>::parse_json_object(const char *str)
{
  str = skip_whitespace(str);

  /*
   * parse '{'
   *
   * parse string (key)
   *
   * parse ':'
   *
   * parse value (serializable, string, array, number, bool or null)
   *
   * parse '}'
   */
  if (!is_end_of_string(str[0]) && str[0] != '{') {
    // TODO: throw json_error instead
    throw std::logic_error("character isn't serializable opening bracket");
  }
  ++str;
  /*
   * call handler callback
   */
  handler_->on_begin_object();

  if (!is_end_of_string(str[0]) && str[0] == '}') {
    /*
     * call handler callback
     */
    ++str;
    handler_->on_end_object();
    // empty serializable
    return;
  }

  // skip white
  str = skip_whitespace();

  do {

    /*
     * call handler callback
     */
    handler_->on_object_key(parse_json_string(str));

    // skip ws
    str = skip_whitespace();

    // read colon
    if (!is_end_of_string(str[0]) && str[0] != ':') {
      // TODO: throw json_error instead
      throw std::logic_error("character isn't colon");
    }

    parse_json_value(++str);

    // skip white
    str = skip_whitespace();

  } while (!is_end_of_string(str[0]) && str[0] == ',');

  if (!is_end_of_string(str[0]) && str[0] != '}') {
    // TODO: throw json_error instead
    throw std::logic_error("not a valid serializable closing bracket");
  }

  /*
   * call handler callback
   */
  handler_->on_end_object();
}

template<class T>
std::string generic_json_parser_ng<T>::parse_json_string(const char *json_str)
{
  return std::string();
}

template < class T >
const char* generic_json_parser_ng<T>::skip_whitespace(const char *str)
{
  while(isspace(*str)) {
    str++;
  }
  return str;
}

template<class T>
bool generic_json_parser_ng<T>::is_end_of_string(char c)
{
  return c == '\0';
}

}

#endif //MATADOR_GENERIC_JSON_PARSER_NG_HPP
