#ifndef REACTOR_GENERIC_JSON_PARSER_HPP
#define REACTOR_GENERIC_JSON_PARSER_HPP

#include <stdexcept>
#include <iostream>

namespace matador {

/**
 * @class generic_json_parser
 * @tparam T The concrete parser class
 *
 * @brief An generic json parser base class
 *
 * This class implements a generic json
 * parser base class. It parses an json
 * input stream and for each json value type
 * a method of parser class T is called.
 * The parser class decides what it will do
 * with the given information.
 */
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
  explicit generic_json_parser(T *h) : handler_(h) {}

  struct real_t {
    double real = 0.0;
    long long integer = 0;
    bool is_real = false;
  };

public:
  virtual ~generic_json_parser() = default;

protected:
  /**
   * @brief Parse the json input stream.
   *
   * Parse the json input stream and call
   * the appropiate callbacks interally.
   *
   * @param in The json input stream
   */
  void parse_json(std::istream &in);

private:
  void parse_json_object(std::istream &in);
  void parse_json_array(std::istream &in);
  std::string parse_json_string(std::istream &in);
  real_t parse_json_number(std::istream &in);
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
generic_json_parser<T>::parse_json(std::istream &in)
{
  // eat whitespace
  in >> std::ws;

  char c = in.peek();

  if (!in.good()) {
    // TODO: throw json_error instead
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
   * parse value (serializable, string, array, number, bool or null)
   *
   * parse '}'
   */
  char c(0);

  c = in.get();
  if (in.good() && c != '{') {
    in.putback(c);
    // TODO: throw json_error instead
    throw std::logic_error("character isn't serializable opening bracket");
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
    // empty serializable
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
      // TODO: throw json_error instead
      throw std::logic_error("character isn't colon");
    }

    parse_json_value(in);

    // skip white
    in >> std::ws;

    c = in.get();
  } while (in && in.good() && c == ',');

  if (c != '}') {
    // TODO: throw json_error instead
    throw std::logic_error("not a valid serializable closing bracket");
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
    // TODO: throw json_error instead
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
    // TODO: throw json_error instead
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
    // TODO: throw json_error instead
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
              // TODO: throw json_error instead
              throw std::logic_error("invalid json character");
            }
          }
          break;
        default:
          // TODO: throw json_error instead
          throw std::logic_error("invalid json character");
      }
    } else {
      value.push_back(c);
    }
  }
  return value;
}

template < class T >
typename generic_json_parser<T>::real_t
generic_json_parser<T>::parse_json_number(std::istream &in)
{
  // skip white
  in >> std::ws;

  // number could be an integer
  // or a floating point
  real_t value;
  in >> value.integer;

  char c = in.peek();

  if (c == '.') {
    // it's a double
    value.is_real = true;
    double real;
    in >> real;

    if (value.integer < 0) {
      value.real = value.integer - real;
    } else {
      value.real = value.integer + real;
    }
    value.integer = 0;
    c = in.peek();
  }
  switch (c) {
    case ' ':
    case ',':
    case ']':
    case '}':
      break;
    default:
      // TODO: throw json_error instead
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
    // TODO: throw json_error instead
    throw std::logic_error("input stream isn't good");
  }

  int i = 1;
  if (c == 't') {
    // check for "true"
    while (in && i < 4) {
      c = in.get();
      if (!in.good()) {
        // TODO: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::true_string[i++] != c) {
        // TODO: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
    return true;

  } else if (c == 'f') {
    // check for "false"
    while (in && i < 5) {
      c = in.get();
      if (!in.good()) {
        // TODO: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::false_string[i++] != c) {
        // TODO: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
    return false;
  } else {
    // TODO: throw json_error instead
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
    // TODO: throw json_error instead
    throw std::logic_error("input stream isn't good");
  }

  int i = 1;
  if (c == 'n') {
    // check for "null"
    while (in && i < 4) {
      c = in.get();
      if (!in.good()) {
        // TODO: throw json_error instead
        throw std::logic_error("input stream isn't good");
      }
      if (generic_json_parser<T>::null_string[i++] != c) {
        // TODO: throw json_error instead
        throw std::logic_error("invalid bool character");
      }
    }
  } else {
    // TODO: throw json_error instead
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
    case '.':
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
      // TODO: throw json_error instead
      throw std::logic_error("unknown json type");
  }
}

}
#endif //REACTOR_GENERIC_JSON_PARSER_HPP
