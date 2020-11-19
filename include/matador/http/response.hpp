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

  struct status_t {
    int value = 0;
    std::string message;
  };

  struct content_t {
    std::size_t length;
    std::string type;
    std::string md5;
    std::string language;
  };

public:

  std::string to_string() const;

  version_t version;
  content_t content_type;

  status_t status;

  string_map_t headers;

  std::string body;
};

}
}

#endif //MATADOR_RESPONSE_HPP
