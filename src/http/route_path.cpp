#include "matador/http/route_path.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"

#include <utility>
#include <regex>

namespace matador {
namespace http {

route_endpoint::route_endpoint(std::string path_spec, std::regex path_regex, http::method_t method, t_request_handler request_handler)
  : path_spec_(std::move(path_spec))
  , path_regex_(std::move(path_regex))
  , method_(method)
  , request_handler_(std::move(request_handler))
{
}

bool route_endpoint::match(const std::string &path, http::method_t method, t_path_param_map &path_params)
{
  if (method_ != method) {
    return false;
  }
  std::smatch what;
  if (!std::regex_match(path, what, path_regex_)) {
    return false;
  }

  std::vector<std::string> path_param_vector;
  for (size_t i = 1; i < what.size(); ++i) {
    path_param_vector.push_back(what[i]);
  }

  return true;
}

http::method_t route_endpoint::method() const
{
  return method_;
}

const std::string &route_endpoint::path_spec() const
{
  return path_spec_;
}

response route_endpoint::execute(const request &req, const t_path_param_map &path_params)
{
  return request_handler_(req, path_params);
}

}
}
