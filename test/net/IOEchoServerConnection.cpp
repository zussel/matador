#include "IOEchoServerConnection.hpp"

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
  stream_.read(buf_, [this, self](int ec, int /*nread*/) {
    if (ec == 0) {
      write();
    }
  });
}

void IOEchoServerConnection::write()
{
  auto self(shared_from_this());
  stream_.write(buf_, [this, self](int ec, int /*nwrite*/) {
    if (ec == 0) {
      buf_.clear();
      read();
    }
  });
}
