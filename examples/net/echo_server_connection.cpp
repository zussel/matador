#include "echo_server_connection.hpp"

#include <matador/utils/buffer_view.hpp>

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
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      log_.info(
        "%s read (bytes: %d)> %s",
        endpoint_.to_string().c_str(), nread, std::string(buf_.data(), buf_.size()).c_str()
      );
      echo_.assign(buf_.data(), nread);
      write();
    }
  });
}

void echo_server_connection::write()
{
  auto self(shared_from_this());

  std::list<matador::buffer_view> data{ matador::buffer_view(echo_) };

  stream_.write(data, [this, self](int ec, int nwrite) {
    if (ec == 0) {
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      read();
    }
  });
}
