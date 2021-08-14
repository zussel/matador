#ifndef MATADOR_ECHOSERVER_HPP
#define MATADOR_ECHOSERVER_HPP

#include "matador/utils/buffer.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"

class EchoServer : public matador::handler
{
public:
  EchoServer();

  void init(matador::tcp::socket sock, matador::tcp::peer endpoint);

  void open() override;

  int handle() const override;

  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override;

  void on_close() override;
  void close() override;

  bool is_ready_write() const override;
  bool is_ready_read() const override;

  bool timeout_called() const;

  std::string name() const override;

private:
  bool on_timeout_called_ = false;
  matador::tcp::socket stream_;
  matador::tcp::peer endpoint_;

  matador::buffer message_;

  matador::logger log_;
};


#endif //MATADOR_ECHOSERVER_HPP
