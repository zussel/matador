#include "IOEchoServer.hpp"
#include "IOEchoServerConnection.hpp"

using namespace matador;

IOEchoServer::IOEchoServer(unsigned short port)
  : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
  , connector_(std::make_shared<connector>())
  , port_(port)
{
}

IOEchoServer::~IOEchoServer()
{
  stop();
}

void IOEchoServer::start()
{
  service_thread_ = std::thread([this] {
    service_.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (2));
  });
}

void IOEchoServer::stop()
{
  if (service_.is_running()) {
    service_.shutdown();
  }
  if (service_thread_.joinable()) {
    service_thread_.join();
  }
}

matador::io_service &IOEchoServer::service()
{
  return service_;
}

void IOEchoServer::accept()
{
  service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<IOEchoServerConnection>(stream, std::move(ep));
    conn->start();
  });
}

void IOEchoServer::connect()
{
  service_.connect(connector_, std::to_string(port_), [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<IOEchoServerConnection>(stream, std::move(ep));
    conn->start();
  });
}
