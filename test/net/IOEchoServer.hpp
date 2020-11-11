#ifndef MATADOR_IOECHOSERVER_HPP
#define MATADOR_IOECHOSERVER_HPP

#include "matador/net/io_service.hpp"

#include <thread>

class IOEchoServer
{
public:
  explicit IOEchoServer(unsigned short port);
  ~IOEchoServer();

  void run();

  matador::io_service& service();

private:
  void prepare_accept();

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;
  std::thread service_thread_;
};


#endif //MATADOR_IOECHOSERVER_HPP
