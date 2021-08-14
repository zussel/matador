#ifndef MATADOR_IOECHOSERVERCONNECTION_HPP
#define MATADOR_IOECHOSERVERCONNECTION_HPP

#include <matador/net/io_stream.hpp>
#include <matador/net/ip.hpp>

#include <memory>
#include <array>

class IOEchoServerConnection : public std::enable_shared_from_this<IOEchoServerConnection>
{
public:
  IOEchoServerConnection(matador::io_stream &stream, matador::tcp::peer endpoint);

  void start();
  void read();
  void write();

private:
  std::array<char, 16384> buf_ = {};
  std::string echo_;
  matador::io_stream &stream_;
  matador::tcp::peer endpoint_;
};


#endif //MATADOR_IOECHOSERVERCONNECTION_HPP
