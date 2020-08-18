#ifndef MATADOR_CONNECTOR_HPP
#define MATADOR_CONNECTOR_HPP

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

class OOS_NET_API connector : public handler
{
public:
  typedef std::function<std::shared_ptr<handler>(tcp::socket sock, tcp::peer endpoint, connector *cnnctr)> make_handler_func;

  explicit connector(make_handler_func on_new_connection);

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
  logger log_;
};

}

#endif //MATADOR_CONNECTOR_HPP
