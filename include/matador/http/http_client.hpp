#ifndef MATADOR_HTTP_CLIENT_HPP
#define MATADOR_HTTP_CLIENT_HPP

#include "matador/http/export.hpp"

#include "matador/http/response.hpp"
#include "matador/http/request.hpp"

#include "matador/net/ip.hpp"
#include "matador/net/io_service.hpp"

#include "matador/logger/logger.hpp"

#include <string>

namespace matador {
namespace http {

/**
 * @brief A http client class
 *
 * This class represents a http client connection.
 * The class is initialized with the remote endpoint
 * url consisting of a hostname or ip address and optional
 * a port <host[:port]>.
 *
 * It provides the four main http commands
 * - GET
 * - PUT
 * - POST
 * - DELETE
 *
 * Once on of the four command is called via provided
 * method a connection is established and the request is
 * send synchronously.
 *
 * Once the answer is received and processed a response
 * object is returned.
 */
class OOS_HTTP_API client
{
public:
  /**
   * Creates a client for the given host
   *
   * @param host Host and optional port string
   */
  explicit client(const std::string &host);

  /**
   * Sends a GET request to the given route at the
   * configured host.
   *
   * @param route Route to be requested
   * @return The response of the request
   */
  response get(const std::string &route);

  /**
   * Sends a POST request to the given route at the
   * configured host with the given body as string.
   *
   * @param route Route to be requested
   * @param body String representing the body of the request
   * @return The response of the request
   */
  response post(const std::string &route, const std::string &body);

  /**
   * Sends a PUT request to the given route at the
   * configured host with the given body as string.
   *
   * @param route Route to be requested
   * @param body String representing the body of the request
   * @return The response of the request
   */
  response put(const std::string &route, const std::string &body);

  /**
   * Sends a DELETE request to the given route at the
   * configured host.
   *
   * @param route Route to be requested
   * @return The response of the request
   */
  response remove(const std::string &route);

private:
  response execute(const request& req);

  void connect(tcp::socket &stream);
  void send_request(tcp::socket &stream, const request &req);
  void read_response(tcp::socket &stream, response &resp);

private:
  std::string host_;
  std::string port_ { "80" };

  logger log_;
};

}
}

#endif //MATADOR_HTTP_CLIENT_HPP
