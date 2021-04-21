#include "matador/http/http_client_connection.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/io_stream.hpp"

namespace matador {
namespace http {

http_client_connection::http_client_connection(request req, response &resp, io_stream &stream, matador::tcp::peer endpoint)
  : log_(matador::create_logger("HttpClientConnection"))
  , stream_(stream)
  , endpoint_(std::move(endpoint))
  , request_(std::move(req))
  , response_(resp)
{}

void http_client_connection::execute()
{
  write();
}

void http_client_connection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      std::string response_string(buf_.data(), nread);
      // parse request and prepare response
      log_.trace("%s: response [%s]", stream_.name().c_str(), response_string.c_str());
      auto result = parser_.parse(response_string, response_);

      if (result == response_parser::FINISH) {
        log_.info("%s: %d %s HTTP/%d.%d", stream_.name().c_str(), response_.status(), http::to_string(response_.status()).c_str(),
                  response_.version().major, response_.version().minor);

        parser_.reset();
        stream_.close_stream();
      } else if (result == response_parser::INVALID){
        log_.debug("invalid response; closing stream");
        response_ = response::bad_request();
        stream_.close_stream();
      } else {
        // not all data read
        log_.debug("not all data was read; continue reading");
        read();
      }
    } else {
      log_.error("error on reading response: %d", ec);
      stream_.close_stream();
    }
  });
}

void http_client_connection::write()
{
  auto self(shared_from_this());

  std::list<buffer_view> data = request_.to_buffers();

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