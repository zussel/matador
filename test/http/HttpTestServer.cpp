#include "HttpTestServer.hpp"

#include "matador/utils/buffer.hpp"

#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request_parser.hpp"

#include "matador/net/acceptor.hpp"


#include <thread>

using namespace matador;

HttpTestServer::HttpTestServer(unsigned short port)
: port_(port)
{}

void HttpTestServer::run()
{
  tcp::acceptor acceptor;
  tcp::peer endpoint { address::v4::any(), port_ };
  acceptor.bind(endpoint);
  acceptor.listen(5);

  auto socket_id = acceptor.id();
  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(socket_id, &read_set);

  running_ = true;

  select(static_cast<int>(acceptor.id())+1, &read_set, nullptr, nullptr, nullptr);

  FD_ISSET(socket_id, &read_set);

  tcp::socket remote;
  acceptor.accept(remote);

  try {
    http::request request;
    http::request_parser::return_t parse_result{};
    http::request_parser parser;
    do {
      buffer result;
      auto nread = remote.receive(result);
      if (nread < 0) {
        continue;
      }
      std::string msg(result.data(), nread);

      parse_result = parser.parse(msg, request);
      if (parse_result == matador::http::request_parser::INVALID) {
        break;
      }
    } while (parse_result != matador::http::request_parser::FINISH);

    auto response = http::response::ok("<h1>hello world</h1>", http::mime_types::TYPE_TEXT_HTML);
    if (parse_result == matador::http::request_parser::INVALID) {
      response = http::response::bad_request();
    }

    std::list<buffer_view> data = response.to_buffers();

    for (const auto &buf: data) {
      remote.send(buf);
    }

  } catch(const std::exception &ex) {
    std::cout << "caught exception: " << ex.what() << "\n";
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  client_id_ = remote.release();
  acceptor.close();

  running_ = false;
}

bool HttpTestServer::is_running() const
{
  return running_;
}

void HttpTestServer::shutdown() const
{
  if (client_id_) {
    os::close(client_id_);
  }
}
