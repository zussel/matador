#ifndef MATADOR_HTTP_CONNECTION_HPP
#define MATADOR_HTTP_CONNECTION_HPP

#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include "matador/utils/buffer.hpp"
#include "matador/utils/optional.hpp"

#include "matador/http/request_parser.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/routing_engine.hpp"

#include <memory>

namespace matador {

class io_stream;

namespace http {

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:
  http_connection(routing_engine &router, matador::io_stream &stream, matador::tcp::peer endpoint);

  void start();
  void read();
  void write();

private:
  optional<routing_engine::route_endpoint_ptr> match(request &req);
  response execute(const request &req, const routing_engine::route_endpoint_ptr &route);

private:
  matador::logger log_;
  matador::buffer buf_;
  matador::io_stream &stream_;
  matador::tcp::peer endpoint_;

  matador::http::request_parser parser_;

  matador::http::routing_engine &router_;

  request request_;
  response response_;
};

}
}

#endif //MATADOR_HTTP_CONNECTION_HPP
