#ifndef MATADOR_SOCKET_INTERRUPTER_HPP
#define MATADOR_SOCKET_INTERRUPTER_HPP

#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

namespace matador {

class socket_interrupter
{
public:
  socket_interrupter();
  ~socket_interrupter() = default;

  int socket_id();

  void interrupt();
  bool reset();

private:
  matador::tcp::socket server_;
  matador::tcp::socket client_;

  matador::logger log_;
};

}

#endif //MATADOR_SOCKET_INTERRUPTER_HPP
