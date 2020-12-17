#ifndef MATADOR_ROUTING_ENGINE_HPP
#define MATADOR_ROUTING_ENGINE_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
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

#include "matador/utils/tree.hpp"

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

public:
  routing_engine();

  void add(const std::string& path, http::http::method_t method, const t_request_handler& request_handler);

  iterator find(const std::string &path, http::http::method_t method);

  iterator match(request &req);

  void dump(std::ostream &out);

  bool valid(const iterator& it) const;

private:

  iterator find_internal(const std::string &path, http::http::method_t method);

  route_endpoint_ptr create_route_endpoint(
    const std::string &path_spec,
    http::method_t method,
    const t_request_handler &request_handler
  );

private:
  std::regex route_regex_;

  std::vector<route_endpoint_ptr> routes_;
};

}
}
#endif //MATADOR_ROUTING_ENGINE_HPP
