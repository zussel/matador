#ifndef MATADOR_IOECHOSERVER_HPP
#define MATADOR_IOECHOSERVER_HPP

#include "matador/net/io_service.hpp"

#include <thread>

class IOEchoServer
{
public:
  explicit IOEchoServer(unsigned short port);
  ~IOEchoServer();

  void start();
  void stop();

  void accept();
  void connect();

  matador::io_service& service();

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;
  std::shared_ptr<matador::connector> connector_;
  std::thread service_thread_;
  unsigned short port_;
};


#endif //MATADOR_IOECHOSERVER_HPP
