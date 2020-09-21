#ifndef MATADOR_ECHO_CLIENT_HPP
#define MATADOR_ECHO_CLIENT_HPP

#include <string>
#include <matador/net/connector.hpp>
#include <matador/logger/log_manager.hpp>
#include <matador/net/io_service.hpp>

class echo_client
{
public:
  explicit echo_client(std::string port);

  void run();

private:
  void prepare_connect();
private:
  matador::io_service service_;
  std::shared_ptr<matador::connector> connector_;
  std::string port_;
};


#endif //MATADOR_ECHO_CLIENT_HPP
