#ifndef MATADOR_HTTP_SERVER_HPP
#define MATADOR_HTTP_SERVER_HPP

#include "matador/utils/tree.hpp"

#include "matador/net/acceptor.hpp"
#include "matador/net/io_service.hpp"

#include "matador/http/http.hpp"
#include "matador/http/route_path.hpp"
#include "matador/http/routing_engine.hpp"

namespace matador {
namespace http {

class server
{
public:
  explicit server(unsigned short port);

  void run();

  template < class RequestHandler >
  void on_get(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::GET, request_handler);
  }

  template < class RequestHandler >
  void on_post(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::POST, request_handler);
  }

  template < class RequestHandler >
  void on_put(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::PUT, request_handler);
  }

  template < class RequestHandler >
  void on_remove(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::DELETE, request_handler);
  }

private:
  template < class RequestHandler >
  void add_route(const std::string &route, http::method_t method, RequestHandler request_handler)
  {
    route_path::t_path_param_map path_params;
    auto r = router_.find(route, method, path_params);
    if (router_.valid(r)) {
      log_.warn("route <%s> already registered for method <%s>", route.c_str(), http::to_string(method).c_str());
      return;
    }
    log_.info("adding route <%s> for method <%s>", route.c_str(), http::to_string(method).c_str());
    router_.add(route, method, request_handler);
  }

private:
  matador::logger log_;
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;

  routing_engine router_;
};
}

}

#endif //MATADOR_HTTP_SERVER_HPP
