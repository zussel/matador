#ifndef MATADOR_HTTPTESTSERVER_HPP
#define MATADOR_HTTPTESTSERVER_HPP

#include "matador/net/os.hpp"

#include <atomic>

class HttpTestServer
{
public:
  explicit HttpTestServer(unsigned short port);

  void run();

  bool is_running() const;
  void shutdown() const;

private:
  unsigned short port_{};
  std::atomic_bool running_{false};
  socket_type client_id_{};
};


#endif //MATADOR_HTTPTESTSERVER_HPP
