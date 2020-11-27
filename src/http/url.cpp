#include "matador/http/url.hpp"

namespace matador {
namespace http {

std::unordered_map<char, const char *> url::char_to_enc_map_({ /* NOLINT */
  { ' ', "%20" },
  { '!', "%21" },
  { '"', "%22" },
  { '#', "%23" },
  { '$', "%24" },
  { '%', "%25" },
  { '&', "%26" },
  { '\'', "%27" },
  { '(', "%28" },
  { ')', "%29" },
  { '*', "%2A" },
  { '+', "%2B" },
  { ',', "%2C" },
  { '-', "%2D" },
  { '.', "%2E" },
  { '/', "%2F" },
  { ':', "%3A" },
  { ';', "%3B" },
  { '<', "%3C" },
  { '=', "%3D" },
  { '>', "%3E" },
  { '?', "%3F" },
  { '@', "%40" },
  { '[', "%5B" },
  { '\\', "%5C" },
  { ']', "%5D" },
  { '{', "%7B" },
  { '|', "%7C" },
  { '}', "%7D" },
});

std::string url::encode(const std::string &str)
{
  return std::string();
}

std::string url::decode(const std::string &str)
{
  return std::string();
}
}
}
