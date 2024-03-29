#ifndef MATADOR_STREAM_HANDLER_HPP
#define MATADOR_STREAM_HANDLER_HPP

#include "matador/utils/buffer_view.hpp"

#include "matador/logger/logger.hpp"

#include "matador/net/export.hpp"
#include "matador/net/handler.hpp"
#include "matador/net/ip.hpp"
#include "matador/net/io_stream.hpp"

#include <atomic>
#include <mutex>

namespace matador {

class handler_creator;

/**
 * The stream_handler class implements the
 * handler and io_stream interface and is
 * used with the io_service to handle socket
 * connections more easily in comparison
 * with the plain reactor boilerplate.
 *
 * Instances of the class are used internally
 * within the io_service hiding the read and write
 * wiring. The user just use the interface
 * provided by the io_service to setup
 * a server.
 */
class OOS_NET_API stream_handler : public handler, public io_stream
{
public:
  typedef std::function<void(tcp::peer, io_stream &)> t_init_handler; /**< Shortcut to the initialize function */

public:
  /**
   * Creates a new stream_handler for the given socket.
   * The acceptor is the link to the creation source where
   * the given init function is called when the handler
   * is initialized
   *
   * @param sock Socket to read and write on
   * @param endpoint Endpoint of the connection
   * @param creator Pointer to the creating handler class
   * @param init_handler Initialize function
   */
  stream_handler(tcp::socket sock, tcp::peer endpoint, handler_creator *creator, t_init_handler init_handler);

  void open() override;
  socket_type handle() const override;
  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override {}
  void on_close() override;
  void close() override;
  bool is_ready_write() const override;
  bool is_ready_read() const override;

  void read(buffer_view buf, t_read_handler read_handler) override;
  void write(std::list<buffer_view> buffers, t_write_handler write_handler) override;

  void close_stream() override;

  tcp::socket &stream() override;

  std::string name() const override;

private:
  logger log_;
  tcp::socket stream_;
  tcp::peer endpoint_;

  std::string name_;

  buffer_view read_buffer_;

  std::list<buffer_view> write_buffers_;

  handler_creator *creator_ = nullptr;

  t_init_handler init_handler_;
  t_read_handler on_read_;
  t_write_handler on_write_;

  std::atomic_bool is_ready_to_read_ { false };
  std::atomic_bool is_ready_to_write_ { false };

  mutable std::mutex mutex_;
};

}
#endif //MATADOR_STREAM_HANDLER_HPP
