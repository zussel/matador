#include "matador/http/http_server.hpp"
#include "matador/http/http_connection.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

server::server(unsigned short port)
  : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<http_connection>(stream, std::move(ep));
    conn->start();
  });
}

void server::run()
{
  service_.run();
}
}
}
