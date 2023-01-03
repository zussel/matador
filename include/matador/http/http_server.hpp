#ifndef MATADOR_HTTP_SERVER_HPP
#define MATADOR_HTTP_SERVER_HPP

#include "matador/http/export.hpp"

#include "matador/net/acceptor.hpp"
#include "matador/net/io_service.hpp"

#include "matador/http/http.hpp"
#include "matador/http/route_endpoint.hpp"
#include "matador/http/routing_engine.hpp"
#include "matador/http/middleware.hpp"

namespace matador {
namespace http {

/**
 * @brief A simple reactor based HTTP server
 *
 * This class implements a single threaded HTTP
 * server based on the reactor implementation.
 *
 * It provides four HTTP methods for which a
 * route (path) and callback pair can be registered.
 *
 * The callback takes a http::request object as input
 * parameter and returns a http::response object.
 *
 * To use the automatic routing of incoming requests
 * by path a routing middleware can be added via
 * calling add_routing_middle_ware().
 */
class OOS_HTTP_API server
{
public:
  /**
   * Creates a HTTP server at the given port.
   *
   * @param port Port on which the server listens for connections.
   */
  explicit server(unsigned short port);

  /**
   * Creates a HTTP server at the given port. The given
   * directory path is used as root directory when serving
   * files.
   *
   * @param port Port on which the server listens for connections.
   * @param dir Directory to serve files from
   */
  server(unsigned short port, const std::string &dir);

  /**
   * Starts the server
   */
  void run();

  /**
   * Stops the server
   */
  void shutdown();

  /**
   * Returns true if the server is running
   *
   * @return True if the server is running
   */
  bool is_running() const;

  /**
   * Registers a HTTP GET method callback for the
   * given route.
   *
   * @tparam RequestHandler Type of the callback
   * @param route The route for the callback
   * @param request_handler The GET callback to be executed
   */
  template < class RequestHandler >
  void on_get(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::GET, request_handler);
  }

  /**
   * Registers a HTTP POST method callback for the
   * given route.
   *
   * @tparam RequestHandler Type of the callback
   * @param route The route for the callback
   * @param request_handler The POST callback to be executed
   */
  template < class RequestHandler >
  void on_post(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::POST, request_handler);
  }

  /**
   * Registers a HTTP PUT method callback for the
   * given route.
   *
   * @tparam RequestHandler Type of the callback
   * @param route The route for the callback
   * @param request_handler The PUT callback to be executed
   */
  template < class RequestHandler >
  void on_put(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::PUT, request_handler);
  }

  /**
   * Registers a HTTP DELETE method callback for the
   * given route.
   *
   * @tparam RequestHandler Type of the callback
   * @param route The route for the callback
   * @param request_handler The DELETE callback to be executed
   */
  template < class RequestHandler >
  void on_remove(const std::string &route, RequestHandler request_handler)
  {
    add_route(route, http::DEL, request_handler);
  }

  /**
   * Adds the given middleware to the middleware
   * pipeline.
   *
   * Note: The last added middleware will be executed
   * at first.
   *
   * @param mware Middleware to add
   */
  void add_middleware(const std::shared_ptr<middleware>& mware);

  /**
   * Adds the routing middleware to the middleware
   * pipeline. It handles that the correct callback
   * is executed depending on the route and the method
   * of the incoming request.
   */
  void add_routing_middleware();

private:
  template < class RequestHandler >
  void add_route(const std::string &path_spec, http::method_t method, RequestHandler request_handler)
  {
    auto r = router_.find(path_spec, method);
    if (router_.valid(r)) {
      log_.warn("path_spec <%s> already registered for method <%s>", path_spec.c_str(), http::to_string(method).c_str());
      return;
    }
    log_.info("adding route <%s> (<%s>)", path_spec.c_str(), http::to_string(method).c_str());
    router_.add(path_spec, method, request_handler);
  }

private:
  matador::logger log_;
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;

  routing_engine router_;

  middleware_pipeline pipeline_;
};
}

}

#endif //MATADOR_HTTP_SERVER_HPP
