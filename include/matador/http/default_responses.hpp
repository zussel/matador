#ifndef MATADOR_DEFAULT_RESPONSES_HPP
#define MATADOR_DEFAULT_RESPONSES_HPP

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

/// @cond MATADOR_DEV

class default_responses
{
public:
  static OOS_HTTP_API const char *OK;
  static OOS_HTTP_API const char *CREATED;
  static OOS_HTTP_API const char *ACCEPTED;
  static OOS_HTTP_API const char *NO_CONTENT;
  static OOS_HTTP_API const char *MULTIPLE_CHOICES;
  static OOS_HTTP_API const char *MOVED_PERMANENTLY;
  static OOS_HTTP_API const char *MOVED_TEMPORARILY;
  static OOS_HTTP_API const char *NOT_MODIFIED;
  static OOS_HTTP_API const char *BAD_REQUEST;
  static OOS_HTTP_API const char *UNAUTHORIZED;
  static OOS_HTTP_API const char *FORBIDDEN;
  static OOS_HTTP_API const char *NOT_FOUND;
  static OOS_HTTP_API const char *INTERNAL_SERVER_ERROR;
  static OOS_HTTP_API const char *NOT_IMPLEMENTED;
  static OOS_HTTP_API const char *BAD_GATEWAY;
  static OOS_HTTP_API const char *SERVICE_UNAVAILABLE;
};

/// @endcond

}
}

#endif //MATADOR_DEFAULT_RESPONSES_HPP
