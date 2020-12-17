#ifndef MATADOR_RESPONSE_HEADER_HPP
#define MATADOR_RESPONSE_HEADER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
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

namespace matador {
namespace http {

class OOS_HTTP_API response_header
{
public:
  static const char *ACCEPT_RANGES;
  static const char *AGE;
  static const char *ALLOW;
  static const char *CACHE_CONTROL;
  static const char *CONNECTION;
  static const char *CONTENT_ENCODING;
  static const char *CONTENT_LANGUAGE;
  static const char *CONTENT_LENGTH;
  static const char *CONTENT_LOCATION;
  static const char *CONTENT_MD5;
  static const char *CONTENT_DISPOSITION;
  static const char *CONTENT_RANGE;
  static const char *CONTENT_SECURITY_POLICY;
  static const char *CONTENT_TYPE;
  static const char *DATE;
  static const char *ETAG;
  static const char *EXPIRES;
  static const char *LAST_MODIFIED;
  static const char *LINK;
  static const char *LOCATION;
  static const char *P3P;
  static const char *PRAGMA;
  static const char *PROXY_AUTHORIZATION;
  static const char *REFRESH;
  static const char *RETRY_AFTER;
  static const char *SERVER;
  static const char *SET_COOKIE;
  static const char *TRAILER;
  static const char *TRANSFER_ENCODING;
  static const char *VARY;
  static const char *VIA;
  static const char *WARNING;
  static const char *WWW_AUTHENTICATE;
};

}
}

#endif //MATADOR_RESPONSE_HEADER_HPP
