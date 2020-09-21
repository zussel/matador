#include "echo_server_connection.hpp"

using namespace matador;

echo_server_connection::echo_server_connection(io_stream &stream, matador::tcp::peer endpoint)
: log_(matador::create_logger("EchoServerConnection"))
, stream_(stream)
, endpoint_(std::move(endpoint))
{}

void echo_server_connection::start()
{
  read();
}

void echo_server_connection::read()
{
  auto self(shared_from_this());
  stream_.read(buf_, [this, self](int ec, int nread) {
    if (ec == 0) {
      log_.info(
        "%s read (bytes: %d)> %s",
        endpoint_.to_string().c_str(), nread, std::string(buf_.data(), buf_.size()).c_str()
      );
      write();
    }
  });
}

void echo_server_connection::write()
{
  auto self(shared_from_this());
  stream_.write(buf_, [this, self](int ec, int nwrite) {
    if (ec == 0) {
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      buf_.reset();
      read();
    }
  });
}
