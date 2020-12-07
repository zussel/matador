#include "matador/http/http_server.hpp"
#include "matador/http/http_connection.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

server::server(unsigned short port)
  : log_(matador::create_logger("HttpServer"))
  , acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
  , router_()
{
  log_.info("creating http server at port %d", port);
  service_.accept(acceptor_, [this](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<http_connection>(router_, stream, std::move(ep));
    conn->start();
  });
}

void server::run()
{
  service_.run();
}

}
}
