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

#include <string>
#include <unordered_map>

namespace matador {
namespace http {

class mime_types
{
public:
  enum types {
    TYPE_IMAGE_GIF,
    TYPE_IMAGE_JPEG,
    TYPE_IMAGE_PNG,
    TYPE_TEXT_HTML,
    TYPE_TEXT_PLAIN,
    TYPE_TEXT_XML,
    TYPE_TEXT_CSS,
    TYPE_APPLICATION_JSON,
    TYPE_APPLICATION_GZIP,
    TYPE_APPLICATION_ZIP,
    TYPE_APPLICATION_JAVASCRIPT,
    TYPE_APPLICATION_X_WWW_FORM_URLENCODED
  };

  static OOS_HTTP_API const char *IMAGE_GIF;
  static OOS_HTTP_API const char *IMAGE_JPEG;
  static OOS_HTTP_API const char *IMAGE_PNG;
  static OOS_HTTP_API const char *TEXT_HTML;
  static OOS_HTTP_API const char *TEXT_PLAIN;
  static OOS_HTTP_API const char *TEXT_XML;
  static OOS_HTTP_API const char *TEXT_CSS;
  static OOS_HTTP_API const char *APPLICATION_JSON;
  static OOS_HTTP_API const char *APPLICATION_GZIP;
  static OOS_HTTP_API const char *APPLICATION_ZIP;
  static OOS_HTTP_API const char *APPLICATION_JAVASCRIPT;
  static OOS_HTTP_API const char *APPLICATION_X_WWW_FORM_URLENCODED;

  static OOS_HTTP_API const char* from_file_extension(const std::string &ext);
  static OOS_HTTP_API const char* from_file_extension(const char *ext);

  static OOS_HTTP_API const char* from_type(types type);

private:
  static std::unordered_map<std::string, const char*> extension_mime_type_map_;
  static std::unordered_map<types, const char*> mime_type_map_;
};

}
}

#endif //MATADOR_MIME_TYPES_HPP
