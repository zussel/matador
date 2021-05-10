#include "matador/http/http_server.hpp"
#include "matador/http/http_server_connection.hpp"

#include "matador/http/middleware/routing_middleware.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/utils/os.hpp"

namespace matador {
namespace http {

server::server(unsigned short port)
  : server(port, "")
{ }

server::server(unsigned short port, const std::string &dir)
  : log_(matador::create_logger("HttpServer"))
  , acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
  , router_()
{
  log_.info("creating http server at port %d serving from [%s]", port, dir.c_str());
  if (!dir.empty()) {
    os::chdir(dir);
  }
  service_.accept(acceptor_, [this](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<http_server_connection>(pipeline_, stream, std::move(ep));
    conn->start();
  });
}

void server::run()
{
  log_.info("serving content at http://localhost:%d", acceptor_->endpoint().port());
  service_.run();
}

void server::shutdown()
{
  service_.shutdown();
}

bool server::is_running() const
{
  return service_.is_running();
}

void server::add_middleware(const std::shared_ptr<middleware>& mware)
{
  pipeline_.add(mware);
}

void server::add_routing_middleware()
{
  pipeline_.add(std::make_shared<middlewares::routing_middleware>(router_));
}
}
}
