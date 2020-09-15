#include "matador/net/stream_handler.hpp"
#include "matador/net/reactor.hpp"

#include "matador/utils/buffer.hpp"

#include "matador/logger/log_manager.hpp"

#include <cerrno>
#include <cstring>

namespace matador {

stream_handler::stream_handler()
  : log_(create_logger("StreamHandler"))
{

}

void stream_handler::open()
{

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
    log_.error("fd %d: error on read: %s", handle(), strerror(errno));
    on_close();
  } else {
    log_.info("received %d bytes", len);
//    data_.assign(chunk.data(), len);
//    log_.info("received data: %s", data_.c_str());
    log_.info("end of data");
  }
}

void stream_handler::on_output()
{

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
  return false;
}

bool stream_handler::is_ready_read() const
{
  return false;
}
}
