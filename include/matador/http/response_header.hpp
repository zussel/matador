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

/**
 * Shortcuts to response header keys
 */
class response_header
{
public:
  static OOS_HTTP_API const char *ACCEPT_RANGES;       /**< Accept ranges header */
  static OOS_HTTP_API const char *AGE;                 /**< Age header */
  static OOS_HTTP_API const char *ALLOW;               /**< Allow header */
  static OOS_HTTP_API const char *CACHE_CONTROL;       /**< Cache control header */
  static OOS_HTTP_API const char *CONNECTION;          /**< Connection header */
  static OOS_HTTP_API const char *CONTENT_ENCODING;    /**< Content encoding header */
  static OOS_HTTP_API const char *CONTENT_LANGUAGE;    /**< Content language header */
  static OOS_HTTP_API const char *CONTENT_LENGTH;      /**< Content length header */
  static OOS_HTTP_API const char *CONTENT_LOCATION;    /**< Content location header */
  static OOS_HTTP_API const char *CONTENT_MD5;         /**< Content MD5 header */
  static OOS_HTTP_API const char *CONTENT_DISPOSITION; /**< Content disposition header */
  static OOS_HTTP_API const char *CONTENT_RANGE;       /**< Content range header */
  static OOS_HTTP_API const char *CONTENT_SECURITY_POLICY; /**< Content security policy header */
  static OOS_HTTP_API const char *CONTENT_TYPE;        /**< Content type header */
  static OOS_HTTP_API const char *DATE;                /**< Date header */
  static OOS_HTTP_API const char *ETAG;                /**< ETAG header */
  static OOS_HTTP_API const char *EXPIRES;             /**< Expires header */
  static OOS_HTTP_API const char *LAST_MODIFIED;       /**< Last modifier header */
  static OOS_HTTP_API const char *LINK;                /**< link header */
  static OOS_HTTP_API const char *LOCATION;            /**< Location header */
  static OOS_HTTP_API const char *P3P;                 /**< P3P header */
  static OOS_HTTP_API const char *PRAGMA;              /**< Pragma header */
  static OOS_HTTP_API const char *PROXY_AUTHORIZATION; /**< Proxy authorization header */
  static OOS_HTTP_API const char *REFRESH;             /**< Refresh header */
  static OOS_HTTP_API const char *RETRY_AFTER;         /**< Retry after header */
  static OOS_HTTP_API const char *SERVER;              /**< Server header */
  static OOS_HTTP_API const char *SET_COOKIE;          /**< Set cookie header */
  static OOS_HTTP_API const char *TRAILER;             /**< Trailer header */
  static OOS_HTTP_API const char *TRANSFER_ENCODING;   /**< Transfer encoding header */
  static OOS_HTTP_API const char *VARY;                /**< Vary header */
  static OOS_HTTP_API const char* VIA;                 /**< Via header */
  static OOS_HTTP_API const char* WARNING;             /**< Warning header */
  static OOS_HTTP_API const char *WWW_AUTHENTICATE;    /**< WWW authenticate header */
};

}
}

#endif //MATADOR_RESPONSE_HEADER_HPP
