#ifndef MATADOR_IO_STREAM_HPP
#define MATADOR_IO_STREAM_HPP

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

#include <functional>

#include "matador/net/ip.hpp"

namespace matador {

class buffer;

class OOS_NET_API io_stream
{
public:
  typedef std::function<void(int ec, int nread)> t_read_handler;
  typedef std::function<void(int ec, int nwrite)> t_write_handler;

  virtual void read(buffer &buf, t_read_handler read_handler) = 0;
  virtual void write(buffer &buf, t_write_handler write_handler) = 0;
  virtual void close_stream() = 0;

  virtual tcp::socket& stream() = 0;
};

}
#endif //MATADOR_IO_STREAM_HPP
