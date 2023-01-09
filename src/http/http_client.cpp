#include "matador/http/http_client.hpp"
#include "matador/http/http_client_connection.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response_parser.hpp"

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

  if (count == 2) {
    host_.assign(parts[0]);
    port_.assign(parts[1]);
  }
}

response client::get(const std::string &route)
{
  return execute(request(http::GET, host_, route));
}

response client::post(const std::string &route, const std::string &body)
{
  auto req = request(http::POST, host_, route);
  req.body(body);

  return execute(req);
}

response client::put(const std::string &route, const std::string &body)
{
  auto req = request(http::PUT, host_, route);
  req.body(body);

  return execute(req);
}

response client::remove(const std::string &route)
{
  return execute(request(http::DEL, host_, route));
}

response client::execute(const request& req)
{
  tcp::socket connection;
  connect(connection);

  if (!connection.is_open()) {
    throw_logic_error("couldn't establish connection to " << host_ << ":" << port_);
  }

  send_request(connection, req);

  if (!connection.is_open()) {
    throw_logic_error("failed to write request to  " << host_ << ":" << port_);
  }

  response resp;
  read_response(connection, resp);

  connection.close();

  return resp;
}

void client::connect(tcp::socket &stream)
{
  tcp::resolver resolver;
  auto endpoints = resolver.resolve(host_, port_);
  for (const auto &endpoint : endpoints) {
    auto ret = matador::connect(stream, endpoint);
    if (ret != 0) {
      if (errno > 0) {
        char error_buffer[1024];
        log_.error("couldn't establish connection to: %s (%s)", endpoint.to_string().c_str(), os::strerror(errno, error_buffer, 1024));
      } else {
        log_.error("skipping connection to: %s", endpoint.to_string().c_str());
      }
      continue;
    } else {
      log_.info("connection established to %s (fd: %d)", endpoint.to_string().c_str(), stream.id());
      break;
    }
  }
}

void client::send_request(tcp::socket &stream, const request &req)
{
  ssize_t bytes_total = 0;
  auto buffer_list = req.to_buffers();
  while  (!buffer_list.empty()) {
    buffer_view &bv = buffer_list.front();

    auto len = stream.send(bv);
    log_.trace("%s: sent %d bytes", host_.c_str(), len);

    if (len == 0) {
      stream.close();
      return;
    } else if (len < 0 && errno != EWOULDBLOCK) {
      char error_buffer[1024];
      log_.error("%s: error on write: %s", host_.c_str(), os::strerror(errno, error_buffer, 1024));
      stream.close();
      return;
    } else if (len < 0 && errno == EWOULDBLOCK) {
      log_.debug("%s: sent %d bytes (blocked)", host_.c_str(), bytes_total);
    } else {
      bytes_total += len;
      bv.bump(len);
      if (bv.full()) {
        buffer_list.pop_front();
      }
    }
  }
}

void client::read_response(tcp::socket &stream, response &resp)
{
  response_parser::return_t parse_result{};
  response_parser parser;
  do {
    buffer result;
    auto nread = stream.receive(result);
    if (nread == 0) {
      stream.close();
      return;
    } else if (nread < 0) {
      char error_buffer[1024];
      log_.error("%s: error on read: %s", host_.c_str(), os::strerror(errno, error_buffer, 1024));
      stream.close();
      return;
    } else {
      std::string msg(result.data(), nread);

      parse_result = parser.parse(msg, resp);
      if (parse_result == matador::http::response_parser::INVALID) {
        resp = response::bad_request();
        break;
      }
    }
  } while (parse_result != matador::http::response_parser::FINISH);
}

}
}