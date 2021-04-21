#include "matador/http/http_server_connection.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/io_stream.hpp"

#include "matador/utils/os.hpp"
#include "matador/utils/buffer_view.hpp"

namespace matador {
namespace http {

http_server_connection::http_server_connection(middleware_pipeline &pipeline, io_stream &stream, matador::tcp::peer endpoint)
  : log_(matador::create_logger("HttpServerConnection"))
  , stream_(stream)
  , endpoint_(std::move(endpoint))
  , pipeline_(pipeline)
{}

void http_server_connection::start()
{
  read();
}

void http_server_connection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      std::string request_string(buf_.data(), nread);
      // parse request and prepare response
      log_.trace("%s: request [%s]", stream_.name().c_str(), request_string.c_str());
      auto result = parser_.parse(request_string, request_);

      if (result == request_parser::FINISH) {
        log_.info(
          "%s: %s %s HTTP/%d.%d",
          stream_.name().c_str(),
          http::to_string(request_.method()).c_str(),
          request_.url().c_str(),
          request_.version().major,
          request_.version().minor
        );

        response_ = process(request_);

        parser_.reset();
        write();
      } else if (result == request_parser::INVALID) {
        log_.debug("invalid request; returning bad request");
        response_ = response::bad_request();
        write();
      } else {
        // not all data read
        log_.debug("not all data was read; continue reading");
        read();
      }
    }
  });
}

void http_server_connection::write()
{
  auto self(shared_from_this());

  std::list<buffer_view> data = response_.to_buffers();

  stream_.write(data, [this, self](int ec, int) {
    if (ec == 0) {
      stream_.close_stream();
    }
  });
}

response http_server_connection::process(request &req) const
{
  return pipeline_.process(req);
}

}
}
