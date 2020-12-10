#include "matador/http/http_connection.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response_header.hpp"
#include "matador/http/mime_types.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/net/io_stream.hpp"

#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

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
  stream_.read(buf_, [this, self](int ec, int nread) {
    if (ec == 0) {
      std::string request_string(buf_.data(), buf_.size());
      log_.info(
        "%s read (bytes: %d)",
        endpoint_.to_string().c_str(), nread
      );
      // parse request and prepare response
      request req;
      auto result = parser_.parse(request_string, req);

      if (result == request_parser::FINISH) {
        log_.info("finished request parsing");
        log_.info("%s %s HTTP/%d.%d", http::to_string(req.method).c_str(), req.url.c_str(), req.version.major, req.version.minor);

        response resp = execute(req);

        auto data = resp.to_string();
        buf_.clear();
        buf_.append(data.c_str(), data.size());
      }

      write();
    }
  });
}

void http_connection::write()
{
  auto self(shared_from_this());
  stream_.write(buf_, [this, self](int ec, int nwrite) {
    if (ec == 0) {
//      log_.info("data to send: %s", std::string(buf_.data(), buf_.size()).c_str());
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      buf_.clear();
      stream_.close_stream();
    }
  });
}

response http_connection::execute(const request &req)
{
  response resp;

  route_path::t_path_param_map path_params;

  log_.info("checking for %s route %s", http::to_string(req.method).c_str(), req.url.c_str());
  auto r = router_.find(req.url, req.method, path_params);

  if (!router_.valid(r)) {
    log_.info("route isn't valid");
    return response::not_found();
  } else {
    log_.info("route is valid");

    return (*r)->execute(req, path_params);
  }
}
}
}