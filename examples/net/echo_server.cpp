#include <matador/net/acceptor.hpp>
#include <matador/logger/log_manager.hpp>
#include <matador/net/io_stream.hpp>
#include "echo_server.hpp"
#include "echo_server_connection.hpp"

using namespace matador;

echo_server::echo_server(unsigned short port)
  : acceptor_(std::make_shared<acceptor>(tcp::peer(address::v4::any(), port)))
{
  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  prepare_accept();
}

void echo_server::run()
{
  service_.run();
}

void echo_server::prepare_accept()
{
  service_.accept(acceptor_, [](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<echo_server_connection>(stream, std::move(ep));
    conn->start();
  });
}
