#ifndef MATADOR_ROUTE_ENDPOINT_HPP
#define MATADOR_ROUTE_ENDPOINT_HPP

#include "matador/http/http.hpp"

namespace matador {
namespace http {

class route_endpoint
{
public:
  route_endpoint() = default;
  route_endpoint(std::string endpoint_name, http::method_t method);

  route_endpoint(const route_endpoint &x) = default;
  route_endpoint(route_endpoint &&x) = default;
  route_endpoint& operator=(const route_endpoint &x) = default;
  route_endpoint& operator=(route_endpoint &&x) = default;
  ~route_endpoint() = default;

  std::string endpoint_name() const;
  http::method_t method() const;

  friend bool operator==(const route_endpoint &a, const route_endpoint &b);
  friend bool operator!=(const route_endpoint &a, const route_endpoint &b);
  friend bool operator<(const route_endpoint &a, const route_endpoint &b);
  friend bool operator<=(const route_endpoint &a, const route_endpoint &b);
  friend bool operator>(const route_endpoint &a, const route_endpoint &b);
  friend bool operator>=(const route_endpoint &a, const route_endpoint &b);

private:
  std::string endpoint_name_;
  http::method_t method_ = http::UNKNOWN;
};


}
}
#endif //MATADOR_ROUTE_ENDPOINT_HPP
