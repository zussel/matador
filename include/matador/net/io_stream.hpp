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
#include <list>

#include "matador/net/ip.hpp"

namespace matador {

class buffer;
class buffer_view;

/**
 * The io stream class is proposed
 * to be used with the io_service class
 * and provides therefore an interface
 * which is used by the io_service
 */
class OOS_NET_API io_stream
{
public:
  typedef std::function<void(int ec, long nread)> t_read_handler;    /**< Short for function to process read data */
  typedef std::function<void(int ec, long nwrite)> t_write_handler;  /**< Short for function to prepare data to write */

  /**
   * This interface is called when data should
   * be read from a socket. Once the date was read
   * the given read handler is called.
   *
   * @param buf Buffer to read the data in
   * @param read_handler Handler to be called when data was read
   */
  virtual void read(const buffer_view &buf, t_read_handler read_handler) = 0;

  /**
   * This interface is called when data should be written
   * to a socket. Once the data was written the given
   * write handler is called.
   *
   * @param buffers List of buffers containing the data to write
   * @param write_handler Handler to be called when the data was written
   */
  virtual void write(std::list<buffer_view> &buffers, t_write_handler write_handler) = 0;

  /**
   * Closes the stream
   */
  virtual void close_stream() = 0;

  /**
   * Returns the underlying stream socket
   * @return
   */
  virtual tcp::socket& stream() = 0;

  virtual std::string name() const = 0;
};

}
#endif //MATADOR_IO_STREAM_HPP
