#ifndef MATADOR_ECHO_SERVER_HPP
#define MATADOR_ECHO_SERVER_HPP

#include <memory>
#include <matador/net/io_service.hpp>

class echo_server
{
public:
  explicit echo_server(unsigned short port);

  void run();

private:
  void prepare_accept();

private:
  matador::io_service service_;
  std::shared_ptr<matador::acceptor> acceptor_;
};


#endif //MATADOR_ECHO_SERVER_HPP
