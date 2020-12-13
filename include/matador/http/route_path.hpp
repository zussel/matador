#ifndef MATADOR_ROUTE_PATH_HPP
#define MATADOR_ROUTE_PATH_HPP

#include "matador/http/http.hpp"

#include <functional>
#include <regex>

namespace matador {
namespace http {

class request;
class response;

typedef std::unordered_map<std::string, std::string> t_path_param_map;
typedef std::function<response(const request&, const t_path_param_map &path_params)> t_request_handler;

/*
 * path
 * path_regex
 * method
 */
class route_endpoint
{
public:
  route_endpoint(std::string path_spec, std::regex path_regex_, http::method_t method, t_request_handler request_handler);

  bool match(const std::string &path, http::method_t method, t_path_param_map &path_params);

  http::method_t method() const;
  const std::string& path_spec() const;

  response execute(const request &req, const t_path_param_map &path_params);

private:
  std::string path_spec_;
  std::regex path_regex_;
  http::method_t method_;
  t_request_handler request_handler_;
};

}
}
#endif //MATADOR_ROUTE_PATH_HPP
