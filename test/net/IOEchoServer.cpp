#include "IOEchoServer.hpp"
#include "IOEchoServerConnection.hpp"

using namespace matador;

IOEchoServer::IOEchoServer(unsigned short port)
  : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
{
  prepare_accept();
}

void IOEchoServer::run()
{
  service_.run();
}

matador::io_service &IOEchoServer::service()
{
  return service_;
}

void IOEchoServer::prepare_accept()
{
  service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<IOEchoServerConnection>(stream, std::move(ep));
    conn->start();
  });
}
