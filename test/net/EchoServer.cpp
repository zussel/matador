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
  if (len == 0) {
    log_.info("socket closed by foreign connection; closing local");
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    // error
    message_.clear();
    on_close();
  } else {
    log_.info("received %d bytes", len);
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

void EchoServer::on_timeout()
{
  on_timeout_called_ = true;
}

void EchoServer::on_close()
{
  close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void EchoServer::close()
{
  if (!stream_.is_open()) {
    return;
  }
  log_.info("closing stream for handler %d", stream_.id());
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

bool EchoServer::timeout_called() const
{
  return on_timeout_called_;
}

std::string EchoServer::name() const
{
  return "EchoServer";
}
