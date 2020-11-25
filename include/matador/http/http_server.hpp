#ifndef MATADOR_HTTP_SERVER_HPP
#define MATADOR_HTTP_SERVER_HPP

#include "matador/utils/tree.hpp"

#include "matador/net/acceptor.hpp"
#include "matador/net/io_service.hpp"

#include "matador/http/http.hpp"
#include "matador/http/route_endpoint.hpp"
#include "matador/http/route_store.hpp"

namespace matador {
namespace http {

class server
{
public:
  server(unsigned short port, std::string context_route);

  void run();

  template < class RequestHandler >
  void on_get(const std::string &route, RequestHandler request_handler)
  {
    if (contains_route(route, http::GET)) {
      return;
    }

    route_store_.add(route, http::GET, request_handler);
  }

  template < class RequestHandler >
  void on_post(const std::string &route, RequestHandler request_handler)
  {
    if (contains_route(route, http::POST)) {
      return;
    }

    route_store_.add(route, http::POST, request_handler);
  }

  template < class RequestHandler >
  void on_put(const std::string &route, RequestHandler request_handler)
  {
    if (contains_route(route, http::PUT)) {
      return;
    }

    route_store_.add(route, http::PUT, request_handler);
  }

  template < class RequestHandler >
  void on_remove(const std::string &route, RequestHandler request_handler)
  {
    if (contains_route(route, http::DELETE)) {
      return;
    }

    route_store_.add(route, http::DELETE, request_handler);
  }

private:
  bool contains_route(const std::string &route, http::method_t method);

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;

  route_store route_store_;
};
}

}

#endif //MATADOR_HTTP_SERVER_HPP
