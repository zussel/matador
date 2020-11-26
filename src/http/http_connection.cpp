#include "matador/http/http_connection.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response_header.hpp"
#include "matador/http/mime_types.hpp"

#include "matador/logger/log_manager.hpp"
#include "matador/logger/logger.hpp"

#include "matador/net/io_stream.hpp"

#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

namespace matador {
namespace http {

http_connection::http_connection(io_stream &stream, matador::tcp::peer endpoint)
  : log_(matador::create_logger("HttpConnection"))
  , stream_(stream)
  , endpoint_(std::move(endpoint))
{

}

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
        "%s read (bytes: %d)> %s",
        endpoint_.to_string().c_str(), nread, request_string.c_str()
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
      log_.info("data to send: %s", std::string(buf_.data(), buf_.size()).c_str());
      log_.info("%s sent (bytes: %d)", endpoint_.to_string().c_str(), nwrite);
      buf_.clear();
      read();
    }
  });
}

response http_connection::execute(const request &)
{
  response resp;

  file f("index.html", "r");

  if (!f.is_open()) {

  }

  // obtain file size:
  fseek (f.stream() , 0 , SEEK_END);
  size_t size = ftell (f.stream());
  rewind (f.stream());

  resp.body.resize(size);

  fread(const_cast<char*>(resp.body.data()), 1, size, f.stream());

  f.close();

  resp.status = http::OK;

  resp.content_type.type = mime_types::TEXT_HTML;
  resp.content_type.length = size;
//  resp.content_type.language = "de";

  resp.version.major = 1;
  resp.version.minor = 1;

  resp.headers.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
  resp.headers.insert(std::make_pair(response_header::CONNECTION, "close"));

  return resp;
}
}
}