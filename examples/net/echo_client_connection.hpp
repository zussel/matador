#ifndef MATADOR_ECHO_CLIENT_CONNECTION_HPP
#define MATADOR_ECHO_CLIENT_CONNECTION_HPP

#include <memory>
#include <matador/net/io_stream.hpp>
#include <matador/utils/buffer.hpp>

class echo_client_connection : public std::enable_shared_from_this<echo_client_connection>
{
public:
  explicit echo_client_connection(matador::io_stream &stream, matador::tcp::peer endpoint);

  void start();

  void read();

  void write();

private:
  matador::logger log_;
  matador::buffer buf_;
  matador::io_stream &stream_;
  matador::tcp::peer endpoint_;
};


#endif //MATADOR_ECHO_CLIENT_CONNECTION_HPP
