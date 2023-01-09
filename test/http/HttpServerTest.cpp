#include "HttpServerTest.hpp"

#include "../net/NetUtils.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/http.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response_parser.hpp"

#include "matador/logger/log_manager.hpp"

#include <chrono>
#include <thread>

using namespace matador;
using namespace ::detail;

HttpServerTest::HttpServerTest()
  : matador::unit_test("http_server", "http server test")
{
  add_test("shutdown", [this]() { test_shutdown(); }, "http server shutdown test");
  add_test("get", [this]() { test_get(); }, "http server get test");
  add_test("post", [this]() { test_post(); }, "http server post test");
  add_test("put", [this]() { test_put(); }, "http server put test");
  add_test("delete", [this]() { test_delete(); }, "http server delete test");
}

void HttpServerTest::initialize()
{
//  default_min_log_level(log_level::LVL_DEBUG);
//  add_log_sink(create_stdout_sink());
}

void HttpServerTest::finalize()
{
//  default_min_log_level(log_level::LVL_INFO);
//  clear_all_log_sinks();
  std::this_thread::sleep_for(std::chrono::milliseconds (300));
}

void HttpServerTest::test_shutdown()
{
  http::server s(8123);

  utils::ThreadRunner runner([&s] {
    s.run();
  }, [&s] {
    s.shutdown();
    utils::wait_until_stopped(s);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(s));

  s.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(s));
}

void HttpServerTest::test_get()
{
  http::server s(8123);

  utils::ThreadRunner runner([&s] {
    s.add_routing_middleware();

    s.on_get("/test/{name}", [](const http::request &req) {
      return http::response::ok("<h1>hello " + req.path_params().at("name") + "</h1>",
                                http::mime_types::TYPE_TEXT_HTML);
    });
    s.run();
  }, [&s] {
    s.shutdown();
    utils::wait_until_stopped(s);
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(s));

  http::request req(http::http::GET, "localhost:8123", "/test/world");
  http::response response;

  send_request(8123, req, response);

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", response.body());
  UNIT_ASSERT_EQUAL(http::http::OK, response.status());

  s.shutdown();
}

void HttpServerTest::test_post()
{
  http::server s(7779);

  utils::ThreadRunner runner([&s] {
    s.add_routing_middleware();

    s.on_post("/test/{name}", [](const http::request &req) {
      return http::response::ok("<h1>" + req.body() + " " + req.path_params().at("name") + "</h1>",
                                http::mime_types::TYPE_TEXT_HTML);
    });
    s.run();
  }, [&s] {
    s.shutdown();
    utils::wait_until_stopped(s);
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(s));

  http::request req(http::http::POST, "localhost:7779", "/test/world");
  req.body("hello");
  http::response response;

  send_request(7779, req, response);

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", response.body());
  UNIT_ASSERT_EQUAL(http::http::OK, response.status());

  s.shutdown();
}

void HttpServerTest::test_put()
{
  http::server s(7779);

  utils::ThreadRunner runner([&s] {
    s.add_routing_middleware();

    s.on_put("/test/{name}", [](const http::request &req) {
      return http::response::ok("<h1>" + req.body() + " " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
    });
    s.run();
  }, [&s] {
    s.shutdown();
    utils::wait_until_stopped(s);
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(s));

  http::request req(http::http::PUT, "localhost:7779", "/test/world");
  req.body("hello");
  http::response response;

  send_request(7779, req, response);

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", response.body());
  UNIT_ASSERT_EQUAL(http::http::OK, response.status());

  s.shutdown();
}

void HttpServerTest::test_delete()
{
  http::server s(7779);

  utils::ThreadRunner runner([&s] {
    s.add_routing_middleware();

    s.on_remove("/test/{name}", [](const http::request &req) {
      return http::response::ok("<h1>hello " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
    });
    s.run();
  }, [&s] {
    s.shutdown();
    utils::wait_until_stopped(s);
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(s));

  http::request req(http::http::DEL, "localhost:7779", "/test/world");
  http::response response;

  send_request(7779, req, response);

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", response.body());
  UNIT_ASSERT_EQUAL(http::http::OK, response.status());

  s.shutdown();
}

void HttpServerTest::send_request(unsigned int port, const http::request &request, http::response &response)
{
  tcp::socket client;

  int ret = client.open(tcp::v4());
  UNIT_ASSERT_FALSE(ret < 0);
  auto srv = tcp::peer(address::v4::loopback(), port);
  ret = client.connect(srv);
  UNIT_ASSERT_FALSE(ret < 0);

  std::list<buffer_view> data = request.to_buffers();

  for (const auto &buf: data) {
    auto size = buf.size();
    size_t ret = client.send(buf);
    UNIT_ASSERT_EQUAL(ret, size);
  }

  http::response_parser::return_t parse_result{};
  http::response_parser parser;
  do {
    buffer result;
    auto nread = client.receive(result);
    std::string msg(result.data(), nread);

    parse_result = parser.parse(msg, response);
    if (parse_result == matador::http::response_parser::INVALID) {
    }
    UNIT_ASSERT_NOT_EQUAL(http::response_parser::INVALID, parse_result);
  } while (parse_result != matador::http::response_parser::FINISH);
  client.close();
}
