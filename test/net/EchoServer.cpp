#include "EchoServer.hpp"

#include "matador/net/reactor.hpp"

using namespace matador;

EchoServer::EchoServer()
  : log_(create_logger("EchoServer"))
{}

void EchoServer::init(matador::tcp::socket sock, matador::tcp::peer endpoint)
{
  stream_ = std::move(sock);
  endpoint_ = std::move(endpoint);
}

void EchoServer::open()
{

}

int EchoServer::handle() const
{
  return stream_.id();
}

void EchoServer::on_input()
{
  auto len = stream_.receive(message_);
  log_.info("received %d bytes", len);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    // error
    message_.clear();
    on_close();
  } else {
    message_.size(len);
  }
}

void EchoServer::on_output()
{
  int len = stream_.send(message_);
  log_.info("sent %d bytes", len);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    message_.clear();
    on_close();
  } else {
    message_.clear();
  }
}

void EchoServer::on_close()
{
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void EchoServer::close()
{
  stream_.close();
}

bool EchoServer::is_ready_write() const
{
  return !message_.empty();
}

bool EchoServer::is_ready_read() const
{
  return message_.empty();
}
