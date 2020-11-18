#ifndef MATADOR_REQUEST_PARSER_HPP
#define MATADOR_REQUEST_PARSER_HPP

#include <string>
#include <cstring>
#include <cstdlib>
#include <memory>

namespace matador {
namespace http {

class request;

class request_parser {
public:
  enum return_t {
    INVALID,
    PARTIAL,
    FINISH
  };

private:
  enum state_t {
    METHOD,
    URL,
    VERSION,
    MAJOR_VERSION,
    MINOR_VERSION,
    HEADER,
    HEADER_KEY,
    HEADER_VALUE_BEGIN,
    HEADER_VALUE,
    HEADER_NEWLINE,
    HEADER_FINISH
  };

  static constexpr const char* CRLF = "\r\n";
  static constexpr const std::size_t CRLF_LEN = strlen("\r\n");

public:
  return_t parse(const std::string &msg, request &r);

  void reset();

private:
  bool parse_method(char c, request &req);
  bool parse_url(char c, request &req);
  bool parse_version(char c);
  bool parse_major_version(char c, request &req);
  bool parse_minor_version(char c, request &req);
  bool parse_header(char c);
  bool parse_header_key(char c);
  bool parse_header_value_begin(char c);
  bool parse_header_value(char c);
  bool parse_header_newline(char c, request &req);
  bool parse_header_finish(char c);

  bool isurlchar(char c) const;

  void insert_header(const std::string &key, const std::string &value, request &req);
  void apply_method(const std::string &method, request &req);

private:
  state_t state_ = METHOD;

  std::size_t http_prefix_index_ = 0;
  std::string current_method_;
  std::string current_key_;
  std::string current_value_;
  std::string blanks_;

  bool skip_blanks_ = false;

  static constexpr const char* URL_SPECIAL_CHAR    = "-._~:/?#[]@!$&'()*+,;=";
  static constexpr const char* HTTP_VERSION_PREFIX = "HTTP";
  static constexpr const std::size_t HTTP_VERSION_PREFIX_LEN = strlen("HTTP");

//  struct delete_char_array { void operator()(char *p) { free(p); }};
//  std::unique_ptr<char[], delete_char_array> line_;
};

}
}

#endif //MATADOR_REQUEST_PARSER_HPP
