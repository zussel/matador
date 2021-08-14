#include "matador/http/mime_types.hpp"

namespace matador {
namespace http {

const char *mime_types::IMAGE_GIF = "image/gif";
const char *mime_types::IMAGE_JPEG = "image/jpeg";
const char *mime_types::IMAGE_PNG = "image/png";
const char *mime_types::TEXT_HTML = "text/html";
const char *mime_types::TEXT_PLAIN = "text/plain";
const char *mime_types::TEXT_XML = "text/xml";
const char *mime_types::TEXT_CSS = "text/css";
const char *mime_types::APPLICATION_JSON = "application/json";
const char *mime_types::APPLICATION_GZIP = "application/gzip";
const char *mime_types::APPLICATION_ZIP = "application/zip";
const char *mime_types::APPLICATION_JAVASCRIPT = "application/javascript";
const char *mime_types::APPLICATION_X_WWW_FORM_URLENCODED = "application/x-www-form-urlencoded";

std::unordered_map<std::string, const char*> mime_types::extension_mime_type_map_{{ /* NOLINT */
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
  { "css", TEXT_CSS },
  { "json", APPLICATION_JSON },
  { "gz", APPLICATION_GZIP },
  { "zip", APPLICATION_ZIP },
  { "js", APPLICATION_JAVASCRIPT }
}};

std::unordered_map<mime_types::types, const char*, detail::enum_class_hash> mime_types::mime_type_map_{{ /* NOLINT */
  { TYPE_IMAGE_GIF, IMAGE_GIF },
  { TYPE_IMAGE_JPEG, IMAGE_JPEG },
  { TYPE_IMAGE_PNG, IMAGE_PNG },
  { TYPE_TEXT_HTML, TEXT_HTML },
  { TYPE_TEXT_PLAIN, TEXT_PLAIN },
  { TYPE_TEXT_XML, TEXT_XML },
  { TYPE_TEXT_CSS, TEXT_CSS },
  { TYPE_APPLICATION_JSON, APPLICATION_JSON },
  { TYPE_APPLICATION_GZIP, APPLICATION_GZIP },
  { TYPE_APPLICATION_ZIP, APPLICATION_ZIP },
  { TYPE_APPLICATION_JAVASCRIPT, APPLICATION_JAVASCRIPT }
}};

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

const char *mime_types::from_type(mime_types::types type)
{
  auto it = mime_type_map_.find(type);
  if (it == mime_type_map_.end()) {
    return TEXT_PLAIN;
  }
  return it->second;
}
}
}
