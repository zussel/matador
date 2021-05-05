#ifndef MATADOR_ROUTING_ENGINE_HPP
#define MATADOR_ROUTING_ENGINE_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/http/route_endpoint.hpp"

#include <memory>
#include <string>
#include <list>

namespace matador {
namespace http {

class request;

class OOS_HTTP_API routing_engine
{
public:
  typedef std::shared_ptr<route_endpoint> route_endpoint_ptr;
  typedef std::vector<route_endpoint_ptr> t_route_vector;

  typedef t_route_vector::iterator iterator;
  typedef t_route_vector::const_iterator const_iterator;

public:
  routing_engine() = default;

  void add(const std::string& path, http::http::method_t method, const t_request_handler& request_handler);

  iterator find(const std::string &path, http::http::method_t method);

  const_iterator match(request &req) const;

  void dump(std::ostream &out);

  bool valid(const const_iterator& it) const;

private:

  iterator find_internal(const std::string &path, http::http::method_t method);

private:

  std::vector<route_endpoint_ptr> routes_;
};

OOS_HTTP_API routing_engine::route_endpoint_ptr create_route_endpoint(
  const std::string &path_spec,
  http::method_t method,
  const t_request_handler &request_handler
);

}
}
#endif //MATADOR_ROUTING_ENGINE_HPP
