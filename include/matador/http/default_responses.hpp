#ifndef MATADOR_DEFAULT_RESPONSES_HPP
#define MATADOR_DEFAULT_RESPONSES_HPP

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

class OOS_HTTP_API default_responses
{
public:
  static const char *OK;
  static const char *CREATED;
  static const char *ACCEPTED;
  static const char *NO_CONTENT;
  static const char *MULTIPLE_CHOICES;
  static const char *MOVED_PERMANENTLY;
  static const char *MOVED_TEMPORARILY;
  static const char *NOT_MODIFIED;
  static const char *BAD_REQUEST;
  static const char *UNAUTHORIZED;
  static const char *FORBIDDEN;
  static const char *NOT_FOUND;
  static const char *INTERNAL_SERVER_ERROR;
  static const char *NOT_IMPLEMENTED;
  static const char *BAD_GATEWAY;
  static const char *SERVICE_UNAVAILABLE;
};
}
}

#endif //MATADOR_DEFAULT_RESPONSES_HPP
