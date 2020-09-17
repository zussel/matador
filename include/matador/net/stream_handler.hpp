#ifndef MATADOR_STREAM_HANDLER_HPP
#define MATADOR_STREAM_HANDLER_HPP

#include "matador/utils/buffer.hpp"

#include "matador/logger/logger.hpp"

#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"
#include "matador/net/io_stream.hpp"

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

class acceptor;

class OOS_NET_API stream_handler : public handler, io_stream
{
public:
  typedef std::function<void(tcp::peer, io_stream &)> t_init_handler;

public:
  stream_handler(tcp::socket sock, tcp::peer endpoint, acceptor *accptr, t_init_handler init_handler);

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

  void read(buffer &buf, t_read_handler read_handler) override;
  void write(buffer &buf, t_write_handler write_handler) override;
  tcp::socket &stream() override;

private:
  logger log_;
  tcp::socket stream_;
  tcp::peer endpoint_;
  buffer buffer_;

  acceptor *acceptor_ = nullptr;

  t_init_handler init_handler_;
  t_read_handler on_read_;
  t_write_handler on_write_;

  bool is_ready_to_read_ = false;
  bool is_ready_to_write_ = false;
};

}
#endif //MATADOR_STREAM_HANDLER_HPP
