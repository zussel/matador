#ifndef MATADOR_REQUEST_PARSER_HPP
#define MATADOR_REQUEST_PARSER_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/http/http.hpp"

#include <string>
#include <cstring>
#include <cstdlib>
#include <memory>

namespace matador {
namespace http {

class request;

class OOS_HTTP_API request_parser {
public:
  enum return_t {
    INVALID,
    PARTIAL,
    FINISH
  };

private:
  enum state_t {
    METHOD,
    URL_PATH,
    URL_QUERY_FIELD,
    URL_QUERY_VALUE,
    URL_FRAGMENT,
    VERSION,
    MAJOR_VERSION,
    MINOR_VERSION,
    HEADER,
    HEADER_KEY,
    HEADER_VALUE_BEGIN,
    HEADER_VALUE,
    HEADER_NEWLINE,
    HEADER_FINISH,
    BODY
  };

  enum hex_parse_t {
    HEX_FIRST,
    HEX_SECOND,
    HEX_FINISHED
  };

  static constexpr const char* CRLF = "\r\n";
  static const std::size_t CRLF_LEN;

public:
  return_t parse(const std::string &msg, request &r);

  void reset();

private:
  bool parse_method(char c, request &req);
  bool parse_url_path(char c, request &req);
  bool parse_url_query_field(char c);
  bool parse_url_query_value(char c, t_string_param_map &params);
  bool parse_url_fragment(char c, request &req);
  bool parse_version(char c);
  bool parse_major_version(char c, request &req);
  bool parse_minor_version(char c, request &req);
  bool parse_header(char c);
  bool parse_header_key(char c);
  bool parse_header_value_begin(char c);
  bool parse_header_value(char c);
  bool parse_header_newline(char c, request &req);
  bool parse_header_finish(char c);

  bool is_url_char(char c) const;
  bool is_hex_char(char c) const;

  void insert_header(const std::string &key, const std::string &value, request &req);
  void apply_method(const std::string &method, request &req);

  void process_body(request &req);

  bool parse_form_data(request &req);

private:
  state_t state_ = METHOD;

  std::size_t http_prefix_index_ = 0;
  std::string current_method_;
  std::string current_key_;
  std::string current_value_;
  std::string current_query_field_;
  std::string current_query_value_;
  std::string blanks_;

  bool skip_blanks_ = false;

  hex_parse_t hex_parse_state_ = HEX_FINISHED;
  std::string hex_str_;

  static constexpr const char* URL_SPECIAL_CHAR    = "-._~:/?#[]@!$&'()*+,;=";
  static constexpr const char* HEX_CHAR            = "ABCDEF0123456789";
  static constexpr const char* HTTP_VERSION_PREFIX = "HTTP";
  static const std::size_t HTTP_VERSION_PREFIX_LEN;
};

}
}

#endif //MATADOR_REQUEST_PARSER_HPP
