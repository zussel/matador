#include "matador/net/socket_interrupter.hpp"

#include "matador/utils/buffer.hpp"

#include "matador/logger/log_manager.hpp"

#ifndef _WIN32
#include <netinet/tcp.h>
#endif

namespace matador {

socket_interrupter::socket_interrupter()
  : client_(tcp::v4())
  , log_(create_logger("SocketInterrupter"))
{
  /*
   * setup acceptor
   * - create socket
   * - set reuse address option
   * - bind to localhost:0 (to get random port)
   * - get address
   * - listen
   */
  tcp::acceptor acceptor;
  acceptor.reuse_address(true);
  tcp::peer local(address::v4::loopback());
  acceptor.bind(local);
  acceptor.listen(SOMAXCONN);

  log_.info("listening for interruptions at %d", acceptor.id());
  /*
   * setup connection
   * - connect to server
   * - accept client
   * - prepare server
   */
  client_.connect(tcp::peer(address::v4::loopback(), local.port()));
  acceptor.accept(server_);
  client_.non_blocking(true);
  client_.options(TCP_NODELAY, true);
}

socket_interrupter::~socket_interrupter()
{
  client_.close();
  server_.close();
}

int socket_interrupter::socket_id()
{
  return server_.id();
}

void socket_interrupter::interrupt()
{
  char val = 0;
  buffer buf;
  buf.append(&val, 1);
  log_.info("fd %d: sending interrupt to fd %d", client_.id(), server_.id());
  client_.send(buf);
}

bool socket_interrupter::reset()
{
  buffer buf;
  log_.info("reading interrupt byte");
  int nread = server_.receive(buf);
  bool interrupted = nread > 0;
  while (nread == (int)buf.capacity()) {
    nread = server_.receive(buf);
  }
  return interrupted;
}

}
