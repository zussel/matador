#ifndef MATADOR_ECHO_SERVER_CONNECTION_HPP
#define MATADOR_ECHO_SERVER_CONNECTION_HPP

#include <matador/net/io_stream.hpp>
#include <matador/net/ip.hpp>
#include <matador/logger/log_manager.hpp>

#include <array>
#include <memory>

class echo_server_connection : public std::enable_shared_from_this<echo_server_connection>
{
public:
  echo_server_connection(matador::io_stream &stream, matador::tcp::peer endpoint);

  void start();
  void read();
  void write();

private:
  matador::logger log_;
  std::array<char, 16384> buf_ = {};
  std::string echo_;
  matador::io_stream &stream_;
  matador::tcp::peer endpoint_;
};


#endif //MATADOR_ECHO_SERVER_CONNECTION_HPP
