#include "matador/http/http_client_connection.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/io_stream.hpp"

namespace matador {
namespace http {

http_client_connection::http_client_connection(/*request req, response &resp, */io_stream &stream, matador::tcp::peer endpoint)
  : log_(matador::create_logger("HttpClientConnection"))
  , stream_(stream)
  , endpoint_(std::move(endpoint))
//  , request_(std::move(req))
//  , response_(resp)
{}

void http_client_connection::execute(const request &req)
{
  write(req);
}

void http_client_connection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int) {
    if (ec == 0) {
      std::string request_string(buf_.data(), buf_.size());
      // parse request and prepare response
      auto result = parser_.parse(request_string, response_);

      if (result) {
        log_.info("%d %s HTTP/%d.%d", response_.status(), http::to_string(response_.status()).c_str(),
                  response_.version().major, response_.version().minor);

        parser_.reset();
      } else {
        response_ = response::bad_request();
      }
    } else {
      log_.error("error on reading response: %d", ec);
    }
    stream_.close_stream();
  });
}

void http_client_connection::write(const request &req)
{
  auto self(shared_from_this());

  std::list<buffer_view> data = req.to_buffers();

  stream_.write(data, [this, self](int ec, int) {
    if (ec == 0) {
      read();
    } else {
      log_.error("error on writing request: %d", ec);
      stream_.close_stream();
    }
  });
}

}
}