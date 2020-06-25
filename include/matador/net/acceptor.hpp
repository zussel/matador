#ifndef MATADOR_ACCEPTOR_HPP
#define MATADOR_ACCEPTOR_HPP

#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include <functional>

namespace matador {

class acceptor : public handler
{
public:
  acceptor(std::function<handler*()> on_new_connection);
  acceptor(const char *hostname, unsigned short port, std::function<handler*()> on_new_connection);
  acceptor(tcp::peer peer, std::function<handler*()> on_new_connection);

  void open() override;
  int handle() const override;
  void on_input() override;
  void on_output() override {}
  void on_except() override {}
  void on_timeout() override {}
  void on_close() override {}

  void close() override;

  bool is_ready_write() const override ;
  bool is_ready_read() const override;

private:
  tcp::acceptor acceptor_;

  std::function<handler*()> on_new_connection_;

  logger log_;
};

}
#endif //MATADOR_ACCEPTOR_HPP
