#ifndef MATADOR_IO_STREAM_HPP
#define MATADOR_IO_STREAM_HPP

#include <functional>

#include "matador/net/ip.hpp"

namespace matador {

class buffer;

class io_stream
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
