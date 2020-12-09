#include "matador/net/stream_handler.hpp"
#include "matador/net/reactor.hpp"

#include "matador/utils/buffer.hpp"

#include "matador/logger/log_manager.hpp"

#include <cerrno>
#include <cstring>

namespace matador {

stream_handler::stream_handler(tcp::socket sock, tcp::peer endpoint, acceptor *accptr, t_init_handler init_handler)
  : log_(create_logger("StreamHandler"))
  , stream_(std::move(sock))
  , endpoint_(std::move(endpoint))
  , acceptor_(accptr)
  , init_handler_(std::move(init_handler))
{
  log_.info("created stream handler with endpoint %s", endpoint.to_string().c_str());
}

stream_handler::stream_handler(tcp::socket sock, tcp::peer endpoint, connector *cnnctr, t_init_handler init_handler)
  : log_(create_logger("StreamHandler"))
  , stream_(std::move(sock))
  , endpoint_(std::move(endpoint))
  , connector_(cnnctr)
  , init_handler_(std::move(init_handler))
{
  log_.info("created stream handler with endpoint %s", endpoint.to_string().c_str());
}

stream_handler::~stream_handler()
{
  log_.info("destroying handler");
}

void stream_handler::open()
{
  init_handler_(endpoint_, *this);
}

int stream_handler::handle() const
{
  return stream_.id();
}

void stream_handler::on_input()
{
  buffer chunk;
  auto len = stream_.receive(chunk);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    char error_buffer[1024];
    log_.error("fd %d: error on read: %s", handle(), os::strerror(errno, error_buffer, 1024));
    is_ready_to_read_ = false;
    on_read_(len, len);
    on_close();
  } else {
    chunk.size(len);
    log_.info("received %d bytes", len);
    is_ready_to_read_ = false;
    read_buffer_->append(chunk);
    on_read_(0, len);
    log_.info("end of data");
  }
}

void stream_handler::on_output()
{
  int len = stream_.send(*write_buffer_);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    char error_buffer[1024];
    log_.error("fd %d: error on write: %s", handle(), os::strerror(errno, error_buffer, 1024));
    on_close();
    is_ready_to_write_ = false;
    on_write_(len, len);
  } else {
    log_.info("sent %d bytes", len);
    is_ready_to_write_ = false;
    on_write_(0, len);
    log_.info("end of data");
  }
}

void stream_handler::on_close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void stream_handler::close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
}

bool stream_handler::is_ready_write() const
{
  return is_ready_to_write_ &&
    write_buffer_ != nullptr &&
    !write_buffer_->empty();
}

bool stream_handler::is_ready_read() const
{
  return is_ready_to_read_ &&
    read_buffer_ != nullptr &&
    read_buffer_->empty() &&
    read_buffer_->capacity() > 0;
}

void stream_handler::read(buffer &buf, t_read_handler read_handler)
{
  on_read_ = std::move(read_handler);
  read_buffer_ = &buf;
  is_ready_to_read_ = true;
}

void stream_handler::write(buffer &buf, t_write_handler write_handler)
{
  on_write_ = std::move(write_handler);
  write_buffer_ = &buf;
  is_ready_to_write_ = true;
}

void stream_handler::close_stream()
{
  on_close();
}

tcp::socket &stream_handler::stream()
{
  return stream_;
}

}
