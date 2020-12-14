#ifndef MATADOR_MIME_TYPES_HPP
#define MATADOR_MIME_TYPES_HPP

#include <string>
#include <unordered_map>

namespace matador {
namespace http {

class mime_types
{
public:
  static const char *IMAGE_GIF;
  static const char *IMAGE_JPEG;
  static const char *IMAGE_PNG;
  static const char *TEXT_HTML;
  static const char *TEXT_PLAIN;
  static const char *TEXT_XML;
  static const char *TEXT_CSS;
  static const char *APPLICATION_JSON;
  static const char *APPLICATION_GZIP;
  static const char *APPLICATION_ZIP;
  static const char *APPLICATION_JAVASCRIPT;

  static const char* from_file_extension(const std::string &ext);
  static const char* from_file_extension(const char *ext);

private:
  static std::unordered_map<std::string, const char*> extension_mime_type_map_;
};

}
}

#endif //MATADOR_MIME_TYPES_HPP
