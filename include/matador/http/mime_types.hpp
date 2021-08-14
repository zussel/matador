#ifndef MATADOR_MIME_TYPES_HPP
#define MATADOR_MIME_TYPES_HPP

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

#include <string>
#include <unordered_map>

namespace matador {
namespace http {

/**
 * @brief This class holds http mime type information
 *
 * This class consists of a mime type enumeration
 * their string counterparts for use in http requests
 * as content type and conversion methods
 */
class mime_types
{
public:
  /**
   * Mime type enumeration
   */
  enum types {
    TYPE_IMAGE_GIF,           /**< image/gif type */
    TYPE_IMAGE_JPEG,          /**< image/jpeg type */
    TYPE_IMAGE_PNG,           /**< image/png type */
    TYPE_TEXT_HTML,           /**< text/html type */
    TYPE_TEXT_PLAIN,          /**< text/plain type */
    TYPE_TEXT_XML,            /**< text/xml type */
    TYPE_TEXT_CSS,            /**< text/css type */
    TYPE_APPLICATION_JSON,    /**< application/json type */
    TYPE_APPLICATION_GZIP,    /**< application/gzip type */
    TYPE_APPLICATION_ZIP,     /**< application/zip type */
    TYPE_APPLICATION_JAVASCRIPT,            /**< application/javascript type */
    TYPE_APPLICATION_X_WWW_FORM_URLENCODED  /**< application/x-www-form-urlencoded type */
  };

  static OOS_HTTP_API const char *IMAGE_GIF;        /**< image/gif type string */
  static OOS_HTTP_API const char *IMAGE_JPEG;       /**< image/jpeg type string */
  static OOS_HTTP_API const char *IMAGE_PNG;        /**< image/png type string */
  static OOS_HTTP_API const char *TEXT_HTML;        /**< text/html type string */
  static OOS_HTTP_API const char *TEXT_PLAIN;       /**< text/plain type string */
  static OOS_HTTP_API const char *TEXT_XML;         /**< text/xml type string */
  static OOS_HTTP_API const char *TEXT_CSS;         /**< text/css type string */
  static OOS_HTTP_API const char *APPLICATION_JSON; /**< application/json type string */
  static OOS_HTTP_API const char *APPLICATION_GZIP; /**< application/gzip type string */
  static OOS_HTTP_API const char *APPLICATION_ZIP;  /**< application/zip type string */
  static OOS_HTTP_API const char *APPLICATION_JAVASCRIPT;           /**< application/javascript type string */
  static OOS_HTTP_API const char *APPLICATION_X_WWW_FORM_URLENCODED;/**< application/x-www-form-urlencoded type string */

  /**
   * Determines the mime type string from
   * a files extension
   *
   * @param ext File extension to be determined
   * @return The mine type string
   */
  static OOS_HTTP_API const char* from_file_extension(const std::string &ext);

  /**
   * Determines the mime type string from
   * a files extension
   *
   * @param ext File extension to be determined
   * @return The mine type string
   */
  static OOS_HTTP_API const char* from_file_extension(const char *ext);

  /**
   * Converts a mime type enum value into
   * its string representation
   *
   * @param type Mime type to convert
   * @return The converted mime type string
   */
  static OOS_HTTP_API const char* from_type(types type);

private:
  static std::unordered_map<std::string, const char*> extension_mime_type_map_;
  static std::unordered_map<types, const char*, detail::enum_class_hash> mime_type_map_;
};

}
}

#endif //MATADOR_MIME_TYPES_HPP
