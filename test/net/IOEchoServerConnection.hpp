#ifndef MATADOR_IOECHOSERVERCONNECTION_HPP
#define MATADOR_IOECHOSERVERCONNECTION_HPP

#include <matador/net/io_stream.hpp>
#include <matador/net/ip.hpp>
#include <matador/utils/buffer.hpp>

#include <memory>

class IOEchoServerConnection : public std::enable_shared_from_this<IOEchoServerConnection>
{
public:
  IOEchoServerConnection(matador::io_stream &stream, matador::tcp::peer endpoint);

  void start();
  void read();
  void write();

private:
  matador::buffer buf_;
  matador::io_stream &stream_;
  matador::tcp::peer endpoint_;

};


#endif //MATADOR_IOECHOSERVERCONNECTION_HPP
