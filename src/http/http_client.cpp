#include "matador/http/http_client.hpp"
#include "matador/http/http_client_connection.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response_parser.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

client::client(const std::string &host)
  : host_(host)
  , connector_(std::make_shared<connector>())
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
  tcp::resolver resolver;
  auto endpoints = resolver.resolve(host_, port_);
  for (const auto &endpoint : endpoints) {
    auto ret = matador::connect(connection, endpoint);
    if (ret != 0) {
      if (errno > 0) {
        char error_buffer[1024];
//        std::cout << "couldn't establish connection to: " << endpoint.to_string().c_str() << " (error: " << os::strerror(errno, error_buffer, 1024) << "\n";
        log_.error("couldn't establish connection to: %s (%s)", endpoint.to_string().c_str(), os::strerror(errno, error_buffer, 1024));
      } else {
//        std::cout << "skipping connection to: " << endpoint.to_string().c_str() << "\n";
        log_.error("skipping connection to: %s", endpoint.to_string().c_str());
      }
      continue;
    } else {
//      std::cout << "connection established to " << endpoint.to_string().c_str() << " (fd: " << connection.id() << ")\n";
      log_.info("connection established to %s (fd: %d)", endpoint.to_string().c_str(), connection.id());
      break;
    }
  }

  if (!connection.is_open()) {
    throw_logic_error("couldn't connect to " << host_ << ":" << port_);
  }

  auto buffer_list = req.to_buffers();
  for (const auto &buf : buffer_list) {
    connection.send(buf);
  }

  response_parser::return_t parse_result{};
  response_parser parser;
  do {
    buffer result;
    auto nread = connection.receive(result);
    if (nread == -1) {
      char errbuf[1024];
      std::cout << "errno: " << os::strerror(errno, errbuf, 1024) << "\n";
      continue;
    }
    std::string msg(result.data(), nread);

    parse_result = parser.parse(msg, response_);
    if (parse_result == matador::http::response_parser::INVALID) {
      response_ = response::bad_request();
      break;
    }
  } while (parse_result != matador::http::response_parser::FINISH);

  connection.close();

//  request_ = std::move(req);
//  service_.connect(connector_, port_, [this](tcp::peer ep, io_stream &stream) {
//    // create echo server connection
//    auto conn = std::make_shared<http_client_connection>(request_, response_, stream, service_, std::move(ep));
//    conn->execute();
//  });

//  service_.run();

  return response_;
}

}
}