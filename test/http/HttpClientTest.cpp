#include "HttpClientTest.hpp"

#include "matador/http/http_client.hpp"
#include "matador/http/request_parser.hpp"

using namespace matador;

void serve(unsigned short port)
{
  tcp::acceptor acceptor;
  tcp::peer endpoint { address::v4::any(), port };
  acceptor.bind(endpoint);
  acceptor.listen(5);

  auto socket_id = acceptor.id();
  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(socket_id, &read_set);

  int ret = select(acceptor.id()+1, &read_set, nullptr, nullptr, nullptr);

  bool ready_to_read = FD_ISSET(socket_id, &read_set);

  tcp::socket remote;
  acceptor.accept(remote);

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

  remote.close();
  acceptor.close();
}

HttpClientTest::HttpClientTest()
  : matador::unit_test("http_client", "http client test")
{
  add_test("get", [this]() { test_get(); }, "http client get test");
  add_test("post", [this]() { test_post(); }, "http client post test");
  add_test("put", [this]() { test_put(); }, "http client put test");
  add_test("delete", [this]() { test_delete(); }, "http client delete test");
}

void HttpClientTest::test_get() {
  std::thread server([]() {
    serve(8123);
  });

  http::client c("localhost:8123");
  auto resp = c.get("/test/world");

  if (server.joinable()) {
    server.join();
  }

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
}

void HttpClientTest::test_post() {
  std::thread server([]() {
    serve(8123);
  });

  http::client c("localhost:8123");
  auto resp = c.post("/test/world", "hello");

  if (server.joinable()) {
    server.join();
  }

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
}

void HttpClientTest::test_put() {
  std::thread server([]() {
    serve(8123);
  });

  http::client c("localhost:8123");
  auto resp = c.put("/test/world", "hello");

  if (server.joinable()) {
    server.join();
  }

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
}

void HttpClientTest::test_delete() {
  std::thread server([]() {
    serve(8123);
  });

  http::client c("localhost:8123");
  auto resp = c.remove("/test/world");

  if (server.joinable()) {
    server.join();
  }

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
}
