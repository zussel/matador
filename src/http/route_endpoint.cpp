#include "matador/http/route_endpoint.hpp"

#include <utility>

namespace matador {
namespace http {

route_endpoint::route_endpoint(std::string endpoint_name, std::string endpoint_path, http::method_t method)
  : endpoint_name_(std::move(endpoint_name))
  , endpoint_path_(std::move(endpoint_path))
  , method_(method)
{}

route_endpoint::route_endpoint(std::string endpoint_name, std::string endpoint_path, http::method_t method, t_request_handler request_handler)
  : endpoint_name_(std::move(endpoint_name))
  , endpoint_path_(std::move(endpoint_path))
  , method_(method)
  , request_handler_(std::move(request_handler))
{}

std::string route_endpoint::endpoint_name() const
{
  return endpoint_name_;
}

const std::string &route_endpoint::endpoint_path() const
{
  return endpoint_path_;
}

void route_endpoint::endpoint_path(const std::string &path)
{
  endpoint_path_ = path;
}

http::method_t route_endpoint::method() const
{
  return method_;
}

void route_endpoint::method(http::method_t method)
{
  method_ = method;
}

bool operator==(const route_endpoint &a, const route_endpoint &b)
{
  return a.endpoint_name_ == b.endpoint_name_ && a.method_ == b.method_;
}

bool operator!=(const route_endpoint &a, const route_endpoint &b)
{
  return !(a == b);
}

bool operator<(const route_endpoint &a, const route_endpoint &b)
{
  return a.endpoint_name_ < b.endpoint_name_ || a.method_ < b.method_;
}

bool operator<=(const route_endpoint &a, const route_endpoint &b)
{
  return a == b || a < b;
}

bool operator>(const route_endpoint &a, const route_endpoint &b)
{
  return !(a <= b);
}

bool operator>=(const route_endpoint &a, const route_endpoint &b)
{
  return a == b || a > b;
}

}
}
