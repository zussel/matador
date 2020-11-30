#ifndef MATADOR_REQUEST_HPP
#define MATADOR_REQUEST_HPP

#include "matador/http/http.hpp"

#include <string>
#include <unordered_map>

namespace matador {
namespace http {

class response;

class request
{
public:

  typedef std::unordered_map<std::string, std::string> string_map_t;

  struct version_t {
    int major = 0;
    int minor = 0;
  };

  struct content_t {
    std::size_t length;
    std::string type;
    std::string md5;
  };

public:
  void execute(response &resp);

  void add_path_param(const std::string &name, const std::string &param);

//  method_t method() const;
//  std::string url() const;
//  version_t version() const;
//  std::string host() const;

private:
  friend class request_parser;

public:
  http::method_t method;
  std::string url;
  version_t version;
  std::string host;

  content_t content;

  string_map_t headers;

  std::string body;

  typedef std::unordered_map<std::string, std::string> t_string_string_map;
  t_string_string_map path_param_map_;
};

}
}

#endif //MATADOR_REQUEST_HPP
