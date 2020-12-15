#ifndef MATADOR_ROUTE_ENDPOINT_HPP
#define MATADOR_ROUTE_ENDPOINT_HPP

#include "matador/http/http.hpp"

#include <functional>
#include <regex>

namespace matador {
namespace http {

class request;
class response;

typedef std::function<response(const request&)> t_request_handler;
typedef std::unordered_map<size_t, std::string> t_size_string_map;

/*
 * path
 * path_regex
 * method
 */
class route_endpoint
{
public:
  route_endpoint(std::string path_spec, const std::string& path_regex, http::method_t method, t_request_handler request_handler, t_size_string_map param_index_map);

  bool match(request &req);

  http::method_t method() const;
  const std::string& path_spec() const;
  const std::string& path_regex() const;

  response execute(const request &req);

private:
  std::string path_spec_;
  std::regex path_regex_;
  std::string path_regex_str_;
  http::method_t method_;
  t_request_handler request_handler_;

  t_size_string_map param_index_map_;
};

}
}
#endif //MATADOR_ROUTE_ENDPOINT_HPP
