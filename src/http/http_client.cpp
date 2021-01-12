#include "matador/http/http_client.hpp"
#include "matador/http/request.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

client::client(const std::string &host)
  : host_(host)
  , log_(matador::create_logger("HttpClient"))
{
  // split host from port (default port is 80)
  std::vector<std::string> parts;
  auto count = matador::split(host, ':', parts);

  if (count > 2 || count == 0) {
    throw_logic_error("invalid host string");
  }

  std::string port { "80" };
  if (count == 2) {
    port = parts.at(1);
  }

  matador::tcp::resolver resolver;

  endpoints_ = resolver.resolve(parts.front(), port);
}

response client::get(const std::string &route)
{
  connect();

  if (stream_.is_open()) {
    throw_logic_error("couldn't connect to " + host_);
  }

  request req(http::GET, route, host_);



  stream_.close();
  return response::no_content();
}

response client::post(const std::string &route, const std::string &body)
{
  return response();
}

response client::put(const std::string &route, const std::string &body)
{
  return response();
}

response client::remove(const std::string &route)
{
  return response();
}

void client::connect()
{
  for (const auto &ep : endpoints_) {
    int ret = matador::connect(stream_, ep);
    if (ret != 0) {
      char error_buffer[1024];
      log_.error("couldn't establish connection to %s: %s", ep.to_string().c_str(),
                 os::strerror(errno, error_buffer, 1024));
      continue;
    } else {
      log_.info("connection established to %s", ep.to_string().c_str());
    }

    stream_.non_blocking(false);
    break;
  }
}

}
}