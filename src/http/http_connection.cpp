#include "matador/http/http_connection.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/io_stream.hpp"

#include "matador/utils/os.hpp"
#include "matador/utils/buffer_view.hpp"

namespace matador {
namespace http {

http_connection::http_connection(routing_engine &router, io_stream &stream, matador::tcp::peer endpoint)
  : log_(matador::create_logger("HttpConnection"))
  , stream_(stream)
  , endpoint_(std::move(endpoint))
  , router_(router)
{}

void http_connection::start()
{
  read();
}

void http_connection::read()
{
  auto self(shared_from_this());
  stream_.read(matador::buffer_view(buf_), [this, self](int ec, int nread) {
    if (ec == 0) {
      std::string request_string(buf_.data(), buf_.size());
      log_.info(
        "%s read (bytes: %d)",
        endpoint_.to_string().c_str(), nread
      );
      // parse request and prepare response
      auto result = parser_.parse(request_string, request_);

      if (result == request_parser::FINISH) {
        log_.info("finished request parsing");
        log_.info("%s %s HTTP/%d.%d", http::to_string(request_.method()).c_str(), request_.url().c_str(), request_.version().major, request_.version().minor);

        auto route = match(request_);

        if (!route.has_value()) {
          log_.info("route %s isn't valid", request_.url().c_str());
          response_ = response::not_found();
        } else {
          response_ = execute(request_, route.value());
        }

        parser_.reset();
//        auto data = response_.to_string();
//        buf_.clear();
//        buf_.append(data.c_str(), data.size());
      }

      write();
    }
  });
}

void http_connection::write()
{
  auto self(shared_from_this());

  std::list<buffer_view> data = response_.to_buffers();

  stream_.write(data, [this, self](int ec, int nwrite) {
    if (ec == 0) {
//      log_.info("data to send: %s", std::string(buf_.data(), buf_.size()).c_str());
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      //buf_.clear();
      stream_.close_stream();
    }
  });
}

optional<routing_engine::route_endpoint_ptr> http_connection::match(request &req)
{
  log_.info("checking for %s route %s", http::to_string(req.method()).c_str(), req.url().c_str());
  auto route = router_.match(req);

  if (router_.valid(route)) {
    return make_optional(*route);
  } else {
    return nullopt;
  }
}

response http_connection::execute(const request &req, const routing_engine::route_endpoint_ptr &route)
{
    log_.debug("route spec: %s (regex: %s)", route->path_spec().c_str(), route->path_regex().c_str());
    return route->execute(req);
}

}
}

#include "matador/utils/os.hpp"
