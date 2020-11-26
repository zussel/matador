#ifndef MATADOR_RESPONSE_HPP
#define MATADOR_RESPONSE_HPP

#include <unordered_map>
#include <string>

#include "matador/http/http.hpp"

namespace matador {
namespace http {

class response
{
  typedef std::unordered_map<std::string, std::string> string_map_t;

  struct version_t {
    int major = 0;
    int minor = 0;
  };

  struct content_t {
    std::size_t length = 0;
    std::string type;
  };

public:

  std::string to_string() const;

  version_t version;
  content_t content_type;

  http::status_t status = http::OK;

  string_map_t headers;

  std::string body;
};

}
}

#endif //MATADOR_RESPONSE_HPP
