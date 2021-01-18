#ifndef MATADOR_HTTP_HPP
#define MATADOR_HTTP_HPP

#include "matador/utils/buffer_view.hpp"

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

#include <unordered_map>
#include <string>

namespace matador {
namespace http {

typedef std::unordered_map<std::string, std::string> t_string_param_map;

class OOS_HTTP_API http
{
public:
  enum method_t {
    UNKNOWN,
    GET,
    POST,
    PUT,
    DELETE,
    OPTIONS,
    HEAD
  };

  static std::string to_string(method_t m);
  static matador::buffer_view to_buffer(method_t m);
  static method_t to_method(const std::string &str);

  enum status_t {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    MOVED_TEMPORARILY = 302,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
  };

  static std::string to_request_string(status_t status);
  static std::string to_string(status_t status);
  static matador::buffer_view to_buffer(status_t status);
  static status_t to_status(const std::string &str);

  struct version {
    int major = 0;
    int minor = 0;
  };

  struct content {
    std::string length {"0" };
    std::string type;
    std::string md5;
    std::string language;
  };

private:
  static std::unordered_map<method_t, std::string> method_string_map_;
  static std::unordered_map<std::string, method_t> string_method_map_;
  static std::unordered_map<status_t, std::string> request_status_string_map_;
  static std::unordered_map<std::string, status_t> string_status_map_;
  static std::unordered_map<status_t, std::string> status_string_map_;
};

}
}

#endif //MATADOR_HTTP_HPP
