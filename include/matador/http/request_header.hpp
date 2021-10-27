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

/**
 * Shortcuts to request header keys
 */
class request_header {
public:
  static OOS_HTTP_API const char* ACCEPT;              /**< Accept header */
  static OOS_HTTP_API const char* ACCEPT_CHARSET;      /**< Accept charset header */
  static OOS_HTTP_API const char* ACCEPT_ENCODING;     /**< Accept encoding header */
  static OOS_HTTP_API const char* ACCEPT_LANGUAGE;     /**< Accept language header */
  static OOS_HTTP_API const char* AUTHORIZATION;       /**< Authorization header */
  static OOS_HTTP_API const char* CACHE_CONTROL;       /**< Cache control header */
  static OOS_HTTP_API const char* CONNECTION;          /**< Connection header */
  static OOS_HTTP_API const char* COOKIE;              /**< Cookie header */
  static OOS_HTTP_API const char* CONTENT_LENGTH;      /**< Content length header */
  static OOS_HTTP_API const char* CONTENT_MD5;         /**< Content MD5 header */
  static OOS_HTTP_API const char* CONTENT_TYPE;        /**< Content type header */
  static OOS_HTTP_API const char* DATE;                /**< Date header */
  static OOS_HTTP_API const char* EXPECT;              /**< Expect header */
  static OOS_HTTP_API const char* FROM;                /**< From header */
  static OOS_HTTP_API const char* HOST;                /**< Host header */
  static OOS_HTTP_API const char* IF_MATCH;            /**< If match header */
  static OOS_HTTP_API const char* IF_MODIFIED_SINCE;   /**< If modified since header */
  static OOS_HTTP_API const char* IF_NONE_MATCH;       /**< If none match header */
  static OOS_HTTP_API const char* IF_RANGE;            /**< If range header */
  static OOS_HTTP_API const char* IF_UNMODIFIED_SINCE; /**< If unmodified since header */
  static OOS_HTTP_API const char* MAX_FORWARDS;        /**< Max forwards header */
  static OOS_HTTP_API const char* PRAGMA;              /**< Pragma header */
  static OOS_HTTP_API const char* PROXY_AUTHORIZATION; /**< Proxy authorization header */
  static OOS_HTTP_API const char* RANGE;               /**< Range header */
  static OOS_HTTP_API const char* REFERER;             /**< Referer header */
  static OOS_HTTP_API const char* TE;                  /**< Te header */
  static OOS_HTTP_API const char* TRANSFER_ENCODING;   /**< Transfer encoding header */
  static OOS_HTTP_API const char* UPGRADE;             /**< Upgrade header */
  static OOS_HTTP_API const char* USER_AGENT;          /**< User agent header */
  static OOS_HTTP_API const char* VIA;                 /**< Via header */
  static OOS_HTTP_API const char* WARNING;             /**< Warning header */
};

}
}

#endif //MATADOR_REQUEST_HEADER_HPP
