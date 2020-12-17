#ifndef MATADOR_REQUEST_HEADER_HPP
#define MATADOR_REQUEST_HEADER_HPP

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

class OOS_HTTP_API request_header {
public:
  static const char* ACCEPT;
  static const char* ACCEPT_CHARSET;
  static const char* ACCEPT_ENCODING;
  static const char* ACCEPT_LANGUAGE;
  static const char* AUTHORIZATION;
  static const char* CACHE_CONTROL;
  static const char* CONNECTION;
  static const char* COOKIE;
  static const char* CONTENT_LENGTH;
  static const char* CONTENT_MD5;
  static const char* CONTENT_TYPE;
  static const char* DATE;
  static const char* EXPECT;
  static const char* FROM;
  static const char* HOST;
  static const char* IF_MATCH;
  static const char* IF_MODIFIED_SINCE;
  static const char* IF_NONE_MATCH;
  static const char* IF_RANGE;
  static const char* IF_UNMODIFIED_SINCE;
  static const char* MAX_FORWARDS;
  static const char* PRAGMA;
  static const char* PROXY_AUTHORIZATION;
  static const char* RANGE;
  static const char* REFERER;
  static const char* TE;
  static const char* TRANSFER_ENCODING;
  static const char* UPGRADE;
  static const char* USER_AGENT;
  static const char* VIA;
  static const char* WARNING;
};

}
}

#endif //MATADOR_REQUEST_HEADER_HPP
