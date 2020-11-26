#ifndef MATADOR_DEFAULT_RESPONSES_HPP
#define MATADOR_DEFAULT_RESPONSES_HPP

namespace matador {
namespace http {

class default_responses
{
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
