#include "matador/http/mime_types.hpp"

namespace matador {
namespace http {

const char *mime_types::IMAGE_GIF = "image/gif";
const char *mime_types::IMAGE_JPEG = "image/jpeg";
const char *mime_types::IMAGE_PNG = "image/png";
const char *mime_types::TEXT_HTML = "text/html";
const char *mime_types::TEXT_PLAIN = "test/plain";
const char *mime_types::TEXT_XML = "test/xml";
const char *mime_types::APPLICATION_JSON = "application/json";
const char *mime_types::APPLICATION_GZIP = "application/gzip";
const char *mime_types::APPLICATION_ZIP = "application/zip";
const char *mime_types::APPLICATION_JAVASCRIPT = "application/javascript";

std::unordered_map<std::string, const char*> mime_types::extension_mime_type_map_({ /* NOLINT */
  { "gif", IMAGE_GIF },
  { "jpg", IMAGE_JPEG },
  { "jpeg", IMAGE_JPEG },
  { "jpe", IMAGE_JPEG },
  { "png", IMAGE_PNG },
  { "html", TEXT_HTML },
  { "htm", TEXT_HTML },
  { "shtml", TEXT_HTML },
  { "txt", TEXT_PLAIN },
  { "xml", TEXT_XML },
  { "json", APPLICATION_JSON },
  { "gz", APPLICATION_GZIP },
  { "zip", APPLICATION_ZIP },
  { "js", APPLICATION_JAVASCRIPT }
});

const char *mime_types::from_file_extension(const std::string &ext)
{
  auto it = extension_mime_type_map_.find(ext);
  if (it == extension_mime_type_map_.end()) {
    return TEXT_PLAIN;
  }
  return it->second;
}

const char *mime_types::from_file_extension(const char *ext)
{
  return from_file_extension(std::string(ext));
}

}
}
