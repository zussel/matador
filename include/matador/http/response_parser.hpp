#ifndef MATADOR_RESPONSE_PARSER_HPP
#define MATADOR_RESPONSE_PARSER_HPP

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

#include <string>

namespace matador {
namespace http {

class response;

class OOS_HTTP_API response_parser
{
public:
  enum return_t {
    INVALID,
    PARTIAL,
    FINISH
  };

public:
  return_t parse(const std::string &msg, response &resp);

  void reset();

private:
  enum state_t {
    METHOD,
    STATUS_CODE,
    STATUS_TEXT,
    URL,
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
    BODY,
    CHUNKED_BODY,
    CHUNKED_BODY_FINISHED
  };

  static constexpr const char* CRLF = "\r\n";
  static const std::size_t CRLF_LEN;

  bool parse_version(char c);
  bool parse_major_version(char c, response &resp);
  bool parse_minor_version(char c, response &resp);
  bool parse_status_code(char c, response &resp);
  bool parse_status_text(char c, response &resp);
  bool parse_header(char c);
  bool parse_header_key(char c);
  bool parse_header_value_begin(char c);
  bool parse_header_value(char c);
  bool parse_header_newline(char c, response &resp);
  bool parse_header_finish(char c);

  bool is_url_char(char c) const;
  bool is_hex_char(char c) const;

  void insert_header(const std::string &key, const std::string &value, response &resp);

private:
  state_t state_ = VERSION;

  std::size_t http_prefix_index_ = 0;
  std::string current_status_;
  std::string current_key_;
  std::string current_value_;
  std::string blanks_;

  bool skip_blanks_ = false;

  static constexpr const char* URL_SPECIAL_CHAR    = "-._~:/?#[]@!$&'()*+,;=";
  static constexpr const char* HEX_CHAR            = "ABCDEF0123456789";
  static constexpr const char* HTTP_VERSION_PREFIX = "HTTP";
  static const std::size_t HTTP_VERSION_PREFIX_LEN;
};

}
}
#endif //MATADOR_RESPONSE_PARSER_HPP
