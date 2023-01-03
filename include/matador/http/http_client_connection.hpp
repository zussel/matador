#ifndef MATADOR_HTTP_CLIENT_CONNECTION_HPP
#define MATADOR_HTTP_CLIENT_CONNECTION_HPP

#include "matador/http/export.hpp"

#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include "matador/utils/buffer.hpp"
#include "matador/utils/optional.hpp"

#include "matador/http/response_parser.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/routing_engine.hpp"

#include <memory>

namespace matador {

class io_stream;
class io_service;

namespace http {

/// @cond MATADOR_DEV

class OOS_HTTP_API http_client_connection : public std::enable_shared_from_this<http_client_connection>
{
public:
  http_client_connection(request req, response &resp, matador::io_stream &stream, matador::io_service &service, matador::tcp::peer endpoint);

  void execute();

  void read();

  void write();

private:
  matador::logger log_;
  std::array<char, 16384> buf_{};
  matador::io_stream &stream_;
  matador::io_service &service_;
  matador::tcp::peer endpoint_;

  matador::http::response_parser parser_;

  request request_;
  response &response_;
};

/// @endcond

}
}

#endif //MATADOR_HTTP_CLIENT_CONNECTION_HPP
