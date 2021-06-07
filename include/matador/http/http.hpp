#ifndef MATADOR_HTTP_HPP
#define MATADOR_HTTP_HPP

#include "matador/utils/buffer_view.hpp"

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

#include "matador/http/enum_class_hash.hpp"

#include <unordered_map>
#include <string>

namespace matador {
namespace http {

/// @cond MATADOR_DEV

typedef std::unordered_map<std::string, std::string> t_string_param_map;

/// @endcond

/**
 * @brief Class consisting of common http protocol definitions
 *
 * This class consists of common http protocol definitions
 * like http method, status or a version and content struct.
 *
 * There are also converting methods for these enums.
 */
class http
{
public:
  /**
   * Enum representing the supported
   * http methods
   */
  enum method_t {
    UNKNOWN,    /**< Unknown method */
    GET,        /**< GET method */
    POST,       /**< POST method */
    PUT,        /**< PUT method */
    DEL,        /**< DELETE method */
    OPTIONS,    /**< OPTIONS method */
    HEAD        /**< HEAD method */
  };

  /**
   * Converts a given http method into a string
   *
   * @param m Http method to convert
   * @return The string representation og the http method
   */
  static OOS_HTTP_API std::string to_string(method_t m);

  /**
   * Converts a given http method into a buffer_view
   *
   * @param m Http method to convert
   * @return The buffer_view representation og the http method
   */
  static OOS_HTTP_API matador::buffer_view to_buffer(method_t m);

  /**
   * Converts a given http method string into
   * its enum representation. If the string is
   * an unknown or unsupported http method
   * the http method enum UNKNOWN is returned
   *
   * @param str String to convert
   * @return The converted http method
   */
  static OOS_HTTP_API method_t to_method(const std::string &str);

  /**
   * Http status codes enumeration
   */
  enum status_t {
    OK = 200,                     /**< OK status code */
    CREATED = 201,                /**< CREATED status code */
    ACCEPTED = 202,               /**< ACCEPTED status code */
    NO_CONTENT = 204,             /**< NO_CONTENT status code */
    MULTIPLE_CHOICES = 300,       /**< MULTIPLE_CHOICES status code */
    MOVED_PERMANENTLY = 301,      /**< MOVED_PERMANENTLY status code */
    MOVED_TEMPORARILY = 302,      /**< MOVED_TEMPORARILY status code */
    NOT_MODIFIED = 304,           /**< NOT_MODIFIED status code */
    BAD_REQUEST = 400,            /**< BAD_REQUEST status code */
    UNAUTHORIZED = 401,           /**< UNAUTHORIZED status code */
    FORBIDDEN = 403,              /**< FORBIDDEN status code */
    NOT_FOUND = 404,              /**< NOT_FOUND status code */
    INTERNAL_SERVER_ERROR = 500,  /**< INTERNAL_SERVER_ERROR status code */
    NOT_IMPLEMENTED = 501,        /**< NOT_IMPLEMENTED status code */
    BAD_GATEWAY = 502,            /**< BAD_GATEWAY status code */
    SERVICE_UNAVAILABLE = 503     /**< SERVICE_UNAVAILABLE status code */
  };

  /**
   * Converts a http status enum value into a request string
   * i.e. "HTTP/1.1 200 OK \r\n"
   *
   * @param status Http status enum to convert
   * @return The request string representing the http status code
   */
  static OOS_HTTP_API std::string to_request_string(status_t status);

  /**
   * Converts a http status enum value into a string
   *
   * @param status Http status enum to convert
   * @return The string representing the http status code
   */
  static OOS_HTTP_API std::string to_string(status_t status);

  /**
   * Converts a http status enum value into a buffer_view
   *
   * @param status Http status enum to convert
   * @return The buffer_view representing the http status code
   */
  static OOS_HTTP_API matador::buffer_view to_buffer(status_t status);

  /**
   * Converts a string representation of a http status into its
   * enum value
   *
   * @param str String to convert
   * @return The converted http status
   */
  static OOS_HTTP_API status_t to_status(const std::string &str);

  /**
   * @brief Version struct
   *
   * The class represents the http version
   * consisting of a major and a minor part.
   */
  struct version {
    int major = 0; /**< Major version number part */
    int minor = 0; /**< Minor version number part */
  };

  /**
   * @brief Content struct
   *
   * This class contains information
   * about the request/response content
   * - content length
   * - content type
   * - content md5 value
   * - content language
   */
  struct content {
    std::string length {"0" };    /**< Content length */
    std::string type;                /**< Content type */
    std::string md5;                 /**< Content md5 value */
    std::string language;            /**< Content language */
  };

private:
  static std::unordered_map<method_t, std::string, detail::enum_class_hash> method_string_map_;
  static std::unordered_map<std::string, method_t> string_method_map_;
  static std::unordered_map<status_t, std::string, detail::enum_class_hash> request_status_string_map_;
  static std::unordered_map<std::string, status_t> string_status_map_;
  static std::unordered_map<status_t, std::string, detail::enum_class_hash> status_string_map_;
};

}
}

#endif //MATADOR_HTTP_HPP
