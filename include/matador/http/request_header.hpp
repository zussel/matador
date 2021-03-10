#ifndef MATADOR_REQUEST_HEADER_HPP
#define MATADOR_REQUEST_HEADER_HPP

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

class request_header {
public:
  static OOS_HTTP_API const char* ACCEPT;
  static OOS_HTTP_API const char* ACCEPT_CHARSET;
  static OOS_HTTP_API const char* ACCEPT_ENCODING;
  static OOS_HTTP_API const char* ACCEPT_LANGUAGE;
  static OOS_HTTP_API const char* AUTHORIZATION;
  static OOS_HTTP_API const char* CACHE_CONTROL;
  static OOS_HTTP_API const char* CONNECTION;
  static OOS_HTTP_API const char* COOKIE;
  static OOS_HTTP_API const char* CONTENT_LENGTH;
  static OOS_HTTP_API const char* CONTENT_MD5;
  static OOS_HTTP_API const char* CONTENT_TYPE;
  static OOS_HTTP_API const char* DATE;
  static OOS_HTTP_API const char* EXPECT;
  static OOS_HTTP_API const char* FROM;
  static OOS_HTTP_API const char* HOST;
  static OOS_HTTP_API const char* IF_MATCH;
  static OOS_HTTP_API const char* IF_MODIFIED_SINCE;
  static OOS_HTTP_API const char* IF_NONE_MATCH;
  static OOS_HTTP_API const char* IF_RANGE;
  static OOS_HTTP_API const char* IF_UNMODIFIED_SINCE;
  static OOS_HTTP_API const char* MAX_FORWARDS;
  static OOS_HTTP_API const char* PRAGMA;
  static OOS_HTTP_API const char* PROXY_AUTHORIZATION;
  static OOS_HTTP_API const char* RANGE;
  static OOS_HTTP_API const char* REFERER;
  static OOS_HTTP_API const char* TE;
  static OOS_HTTP_API const char* TRANSFER_ENCODING;
  static OOS_HTTP_API const char* UPGRADE;
  static OOS_HTTP_API const char* USER_AGENT;
  static OOS_HTTP_API const char* VIA;
  static OOS_HTTP_API const char* WARNING;
};

}
}

#endif //MATADOR_REQUEST_HEADER_HPP
