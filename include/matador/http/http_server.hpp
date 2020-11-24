#ifndef MATADOR_HTTP_SERVER_HPP
#define MATADOR_HTTP_SERVER_HPP

#include "matador/utils/tree.hpp"

#include "matador/net/acceptor.hpp"
#include "matador/net/io_service.hpp"

#include "matador/http/http.hpp"
#include "matador/http/route_endpoint.hpp"

namespace matador {
namespace http {

class request;
class response;

class server
{
public:
  typedef std::function<void(const request&, response&)> t_request_handler;

  explicit server(unsigned short port);

  void run();

  void get(const std::string &route, t_request_handler handler);
  void post(const std::string &route, t_request_handler handler);
  void put(const std::string &route, t_request_handler handler);
  void remove(const std::string &route, t_request_handler handler);

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;

  typedef std::map<http::method_t, tree<route_endpoint>> t_method_route_map;
  t_method_route_map method_route_map_;
};
}

}

#endif //MATADOR_HTTP_SERVER_HPP
