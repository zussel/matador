#ifndef MATADOR_RESPONSE_HEADER_HPP
#define MATADOR_RESPONSE_HEADER_HPP

namespace matador {
namespace http {

class response_header
{
public:
  static constexpr const char *ACCEPT_RANGES = "Accept-Ranges";
  static constexpr const char *AGE = "Age";
  static constexpr const char *ALLOW = "Allow";
  static constexpr const char *CACHE_CONTROL = "Cache-Control";
  static constexpr const char *CONNECTION = "Connection";
  static constexpr const char *CONTENT_ENCODING = "Content-Encoding";
  static constexpr const char *CONTENT_LANGUAGE = "Content-Language";
  static constexpr const char *CONTENT_LENGTH = "Content-Length";
  static constexpr const char *CONTENT_LOCATION = "Content-Location";
  static constexpr const char *CONTENT_MD5 = "Content-MD5";
  static constexpr const char *CONTENT_DISPOSITION = "Content-Disposition";
  static constexpr const char *CONTENT_RANGE = "Content-Range";
  static constexpr const char *CONTENT_SECURITY_POLICY = "Content-Security-Policy";
  static constexpr const char *CONTENT_TYPE = "Content-Type";
  static constexpr const char *DATE = "Date";
  static constexpr const char *ETAG = "ETag";
  static constexpr const char *EXPIRES = "Expires";
  static constexpr const char *LAST_MODIFIED = "Last-Modified";
  static constexpr const char *LINK = "Link";
  static constexpr const char *LOCATION = "Location";
  static constexpr const char *P3P = "P3P";
  static constexpr const char *PRAGMA = "Pragma";
  static constexpr const char *PROXY_AUTHORIZATION = "Proxy-Authorization";
  static constexpr const char *REFRESH = "Refresh";
  static constexpr const char *RETRY_AFTER = "Retry-After";
  static constexpr const char *SERVER = "Server";
  static constexpr const char *SET_COOKIE = "Set-Cookie";
  static constexpr const char *TRAILER = "Trailer";
  static constexpr const char *TRANSFER_ENCODING = "Transfer-Encoding";
  static constexpr const char *VARY = "Vary";
  static constexpr const char *VIA = "Via";
  static constexpr const char *WARNING = "Warning";
  static constexpr const char *WWW_AUTHENTICATE = "WWW-Authenticate";
};

}
}

#endif //MATADOR_RESPONSE_HEADER_HPP
