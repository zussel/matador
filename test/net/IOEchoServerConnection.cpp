#include "IOEchoServerConnection.hpp"

#include "matador/utils/buffer_view.hpp"

IOEchoServerConnection::IOEchoServerConnection(matador::io_stream &stream, matador::tcp::peer endpoint)
  : stream_(stream)
  , endpoint_(std::move(endpoint))
{}

void IOEchoServerConnection::start()
{
  read();
}

void IOEchoServerConnection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      echo_.assign(buf_.data(), nread);
      write();
    }
  });
}

void IOEchoServerConnection::write()
{
  auto self(shared_from_this());

  std::list<matador::buffer_view> data { matador::buffer_view(echo_) };

  stream_.write(data, [this, self](int ec, int /*nwrite*/) {
    if (ec == 0) {
      read();
    }
  });
}
