#include "matador/http/route_endpoint.hpp"

#include <utility>

namespace matador {
namespace http {

route_endpoint::route_endpoint(std::string endpoint_name, http::method_t method)
  : endpoint_name_(std::move(endpoint_name))
  , method_(method)
{}

std::string route_endpoint::endpoint_name() const
{
  return endpoint_name_;
}

http::method_t route_endpoint::method() const
{
  return method_;
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
