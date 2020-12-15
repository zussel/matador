#ifndef MATADOR_ROUTING_ENGINE_HPP
#define MATADOR_ROUTING_ENGINE_HPP

#include "matador/http/route_endpoint.hpp"

#include "matador/utils/tree.hpp"

#include <string>
#include <list>

namespace matador {
namespace http {

class request;

class routing_engine
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
