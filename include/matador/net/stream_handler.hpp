#ifndef MATADOR_STREAM_HANDLER_HPP
#define MATADOR_STREAM_HANDLER_HPP

#include "matador/logger/logger.hpp"

#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"

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

namespace matador {

class OOS_NET_API stream_handler : public handler
{
public:
  stream_handler();

  void open() override;
  int handle() const override;
  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override {}
  void on_close() override;
  void close() override;
  bool is_ready_write() const override;
  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  logger log_;
//  std::function<void
};

}
#endif //MATADOR_STREAM_HANDLER_HPP
