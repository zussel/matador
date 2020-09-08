#ifndef MATADOR_ACCEPTOR_HPP
#define MATADOR_ACCEPTOR_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
    #define OOS_NET_API __declspec(dllexport)
    #define EXPIMP_NET_TEMPLATE
  #else
    #define OOS_NET_API __declspec(dllimport)
    #define EXPIMP_NET_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include <functional>

namespace matador {

class OOS_NET_API acceptor : public handler
{
public:
  typedef std::function<std::shared_ptr<handler>(tcp::socket sock, tcp::peer endpoint, acceptor *accptr)> make_handler_func;

  acceptor();
  acceptor(const tcp::peer& endpoint, make_handler_func on_new_connection);

  void accecpt(const tcp::peer& endpoint, make_handler_func on_new_connection);

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
  tcp::peer endpoint_;

  make_handler_func make_handler_;

  logger log_;

  tcp::peer create_client_endpoint() const;
};

}
#endif //MATADOR_ACCEPTOR_HPP
