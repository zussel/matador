#include "matador/utils/url.hpp"
#include "matador/utils/stream.hpp"

namespace matador {

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
  std::string result;
  // replace special characters with html characters
  for (char i : str) {
    auto c = char_to_enc_map_.find(i);
    if (c == char_to_enc_map_.end()) {
      result += i;
    } else {
      result.append(c->second);
    }
  }
  return result;
}

bool url::decode(const std::string &str, std::string &decoded)
{
  decoded.clear();
  // replace html
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] != '%') {
      decoded += str[i];
    } else if (str[i] == '+') {
      decoded += ' ';
    } else {
      if (i + 2 >= str.size()) {
        return false;
      }
      // get hex string
      auto hexstr = str.substr(i + 1, 2);
      char *err;
      char c = static_cast<char>(std::strtol(hexstr.c_str(), &err, 16));
      decoded += c;
      i += 2;
    }
  }
  return true;
}
}
