#ifndef MATADOR_ROUTE_ENDPOINT_HPP
#define MATADOR_ROUTE_ENDPOINT_HPP

#include "matador/http/http.hpp"

namespace matador {
namespace http {

class route_endpoint
{
public:
  route_endpoint() = default;
  route_endpoint(std::string endpoint_name, std::string endpoint_path, http::method_t method);

  route_endpoint(const route_endpoint &x) = default;
  route_endpoint(route_endpoint &&x) = default;
  route_endpoint& operator=(const route_endpoint &x) = default;
  route_endpoint& operator=(route_endpoint &&x) = default;
  ~route_endpoint() = default;

  std::string endpoint_name() const;

  const std::string &endpoint_path() const;
  void endpoint_path(const std::string &path);

  http::method_t method() const;
  void method(http::method_t method);

  friend bool operator==(const route_endpoint &a, const route_endpoint &b);
  friend bool operator!=(const route_endpoint &a, const route_endpoint &b);
  friend bool operator<(const route_endpoint &a, const route_endpoint &b);
  friend bool operator<=(const route_endpoint &a, const route_endpoint &b);
  friend bool operator>(const route_endpoint &a, const route_endpoint &b);
  friend bool operator>=(const route_endpoint &a, const route_endpoint &b);

private:
  std::string endpoint_name_;
  std::string endpoint_path_;
  http::method_t method_ = http::UNKNOWN;
};


}
}
#endif //MATADOR_ROUTE_ENDPOINT_HPP
