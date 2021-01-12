#include "matador/http/http_client.hpp"
#include "matador/http/request.hpp"
#include "matador/http/request_header.hpp"

#include "matador/logger/log_manager.hpp"

#include <chrono>

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
  req.add_header(request_header::HOST, host_);

  auto buffers = req.to_buffers();

  ssize_t bytes_total = 0;
  auto start = std::chrono::high_resolution_clock::now();
  while (!buffers.empty()) {
    buffer_view &bv = buffers.front();

    auto len = stream_.send(bv);

    if (len == 0) {
      close();
    } else if (len < 0 && errno != EWOULDBLOCK) {
      char error_buffer[1024];
      log_.error("fd %d: error on write: %s", stream_.id(), os::strerror(errno, error_buffer, 1024));
      close();
    } else if (len < 0 && errno == EWOULDBLOCK) {
      log_.info("%s: sent %d bytes (blocked)", endpoint_.to_string().c_str(), bytes_total);
    } else {
      bytes_total += len;
      bv.bump(len);
      if (bv.full()) {
        buffers.pop_front();
      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  log_.info("%s: sent %d bytes (%dµs)", endpoint_.to_string().c_str(), bytes_total, elapsed);

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
    endpoint_ = ep;
    break;
  }
}

void client::close()
{
  if (stream_.is_open()) {
    stream_.close();
  }
}

}
}