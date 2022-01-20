#include "echo_client.hpp"
#include "echo_client_connection.hpp"

using namespace matador;

echo_client::echo_client(std::string port)
: connector_(std::make_shared<matador::connector>())
, port_(std::move(port))
{
  matador::add_log_sink(matador::create_file_sink("log/client.log"));
  //matador::add_log_sink(matador::create_stdout_sink());

  prepare_connect();
}

void echo_client::run()
{
  service_.run();
}

void echo_client::prepare_connect()
{
  service_.connect(connector_, port_, [this](tcp::peer ep, io_stream &stream) {
    // create echo server connection
    auto conn = std::make_shared<echo_client_connection>(stream, service_, std::move(ep));
    conn->start();
  });
}
