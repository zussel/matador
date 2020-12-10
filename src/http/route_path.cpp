#include "matador/http/route_path.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"

#include <utility>
#include <regex>

namespace matador {
namespace http {

route_path::route_path(std::string endpoint_name, std::string endpoint_path, http::method_t method)
  : endpoint_name_(std::move(endpoint_name))
  , endpoint_path_(std::move(endpoint_path))
  , method_(method)
{
  if (endpoint_name_.empty()) {
    return;
  }
  std::regex re(R"(\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))");

  std::smatch what;

  if (!std::regex_match(endpoint_name_, what, re)) {
    throw std::logic_error("invalid route path segment: " + endpoint_name_);
  }

  // group 1 -> path param
  // group 1 + 2 -> path param restricted with regex
  // group 4 -> plain path element
}

route_path::route_path(std::string endpoint_name, std::string endpoint_path, http::method_t method, t_request_handler request_handler)
  : endpoint_name_(std::move(endpoint_name))
  , endpoint_path_(std::move(endpoint_path))
  , method_(method)
  , request_handler_(std::move(request_handler))
{}

bool route_path::match(const std::string &path, t_path_param_map &)
{
  return endpoint_name() == path;
}

std::string route_path::endpoint_name() const
{
  return endpoint_name_;
}

const std::string &route_path::endpoint_path() const
{
  return endpoint_path_;
}

void route_path::endpoint_path(const std::string &path)
{
  endpoint_path_ = path;
}

http::method_t route_path::method() const
{
  return method_;
}

void route_path::method(http::method_t method)
{
  method_ = method;
}

response route_path::execute(const request &req, const route_path::t_path_param_map &path_params)
{
  return request_handler_(req, path_params);
}

bool operator==(const route_path &a, const route_path &b)
{
  return a.endpoint_name_ == b.endpoint_name_ && a.method_ == b.method_;
}

bool operator!=(const route_path &a, const route_path &b)
{
  return !(a == b);
}

bool operator<(const route_path &a, const route_path &b)
{
  return a.endpoint_name_ < b.endpoint_name_ || a.method_ < b.method_;
}

bool operator<=(const route_path &a, const route_path &b)
{
  return a == b || a < b;
}

bool operator>(const route_path &a, const route_path &b)
{
  return !(a <= b);
}

bool operator>=(const route_path &a, const route_path &b)
{
  return a == b || a > b;
}

path_param_route_path::path_param_route_path(std::string param_name, std::string endpoint_name,
                                             std::string endpoint_path, http::method_t method,
                                             route_path::t_request_handler request_handler)
  : route_path(std::move(endpoint_name), std::move(endpoint_path), method, std::move(request_handler))
  , param_name_(std::move(param_name))
{}

bool path_param_route_path::match(const std::string &path, t_path_param_map &path_params)
{
  path_params.insert(std::make_pair(param_name_, path));
  return true;
}

regex_path_param_route_path::regex_path_param_route_path(const std::string& reg, std::string param_name,
                                                         std::string endpoint_name, std::string endpoint_path,
                                                         http::method_t method,
                                                         route_path::t_request_handler request_handler)
  : path_param_route_path(std::move(param_name), std::move(endpoint_name), std::move(endpoint_path), method, std::move(request_handler))
  , param_regex_("(" + reg + ")")
{

}

bool regex_path_param_route_path::match(const std::string &path, t_path_param_map &path_params)
{
  std::smatch m;

  if (!std::regex_match(path, m, param_regex_)) {
    return false;
  }
  path_params.insert(std::make_pair(param_name_, m[1].str()));

  return true;
}

static_file_path::static_file_path(std::string file_pattern, std::string endpoint_name,
                                   std::string endpoint_path, http::method_t method,
                                   route_path::t_request_handler request_handler)
  : route_path(std::move(endpoint_name), std::move(endpoint_path), method, std::move(request_handler))
  , file_pattern_(std::move(file_pattern))
{ }

bool static_file_path::match(const std::string &path, route_path::t_path_param_map &path_params)
{
  return true;
}

}
}
