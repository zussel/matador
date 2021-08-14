#ifndef MATADOR_RESPONSE_HEADER_HPP
#define MATADOR_RESPONSE_HEADER_HPP

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

namespace matador {
namespace http {

class response_header
{
public:
  static OOS_HTTP_API const char *ACCEPT_RANGES;
  static OOS_HTTP_API const char *AGE;
  static OOS_HTTP_API const char *ALLOW;
  static OOS_HTTP_API const char *CACHE_CONTROL;
  static OOS_HTTP_API const char *CONNECTION;
  static OOS_HTTP_API const char *CONTENT_ENCODING;
  static OOS_HTTP_API const char *CONTENT_LANGUAGE;
  static OOS_HTTP_API const char *CONTENT_LENGTH;
  static OOS_HTTP_API const char *CONTENT_LOCATION;
  static OOS_HTTP_API const char *CONTENT_MD5;
  static OOS_HTTP_API const char *CONTENT_DISPOSITION;
  static OOS_HTTP_API const char *CONTENT_RANGE;
  static OOS_HTTP_API const char *CONTENT_SECURITY_POLICY;
  static OOS_HTTP_API const char *CONTENT_TYPE;
  static OOS_HTTP_API const char *DATE;
  static OOS_HTTP_API const char *ETAG;
  static OOS_HTTP_API const char *EXPIRES;
  static OOS_HTTP_API const char *LAST_MODIFIED;
  static OOS_HTTP_API const char *LINK;
  static OOS_HTTP_API const char *LOCATION;
  static OOS_HTTP_API const char *P3P;
  static OOS_HTTP_API const char *PRAGMA;
  static OOS_HTTP_API const char *PROXY_AUTHORIZATION;
  static OOS_HTTP_API const char *REFRESH;
  static OOS_HTTP_API const char *RETRY_AFTER;
  static OOS_HTTP_API const char *SERVER;
  static OOS_HTTP_API const char *SET_COOKIE;
  static OOS_HTTP_API const char *TRAILER;
  static OOS_HTTP_API const char *TRANSFER_ENCODING;
  static OOS_HTTP_API const char *VARY;
  static OOS_HTTP_API const char *VIA;
  static OOS_HTTP_API const char *WARNING;
  static OOS_HTTP_API const char *WWW_AUTHENTICATE;
};

}
}

#endif //MATADOR_RESPONSE_HEADER_HPP
