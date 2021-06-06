#include "HttpServerTest.hpp"

#include "../net/NetUtils.hpp"

#include "matador/http/http_server.hpp"
#include "matador/http/http_client.hpp"
#include "matador/http/response.hpp"

#include "matador/logger/log_manager.hpp"

#include <chrono>
#include <thread>

using namespace matador;
using namespace ::detail;

template < class T >
class ThreadWrapper
{
public:
  explicit ThreadWrapper(T &actor) : actor_(actor) {}
  ~ThreadWrapper()
  {
    if (actor_.is_running()) {
      stop();
    }
    if (reactor_thread_.joinable()) {
      reactor_thread_.join();
    }
  }

  void start()
  {
    reactor_thread_ = std::thread([this] {
      actor_.run();
      // sleep for some seconds to ensure valid thread join
      std::this_thread::sleep_for(std::chrono::seconds (2));
    });
  }
  void stop()
  {
    actor_.shutdown();
  }

  T& get()
  {
    return actor_;
  }

private:
  std::thread reactor_thread_;
  T& actor_;
};

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
  //add_log_sink(create_stdout_sink());
}

void HttpServerTest::finalize()
{
}

void HttpServerTest::test_shutdown()
{
  http::server s(8123);

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void HttpServerTest::test_get()
{
  http::server s(8123);
  s.add_routing_middleware();

  s.on_get("/test/{name}", [](const http::request &req) {
    return http::response::ok("<h1>hello " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
  });

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (400));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  http::client c("localhost:8123");
  auto resp = c.get("/test/world");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  std::this_thread::sleep_for(std::chrono::seconds (1));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void HttpServerTest::test_post()
{
  http::server s(7779);
  s.add_routing_middleware();

  s.on_post("/test/{name}", [](const http::request &req) {
    return http::response::ok("<h1>" + req.body() + " " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
  });

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (400));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  http::client c("localhost:7779");
  auto resp = c.post("/test/world", "hello");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  std::this_thread::sleep_for(std::chrono::seconds (1));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void HttpServerTest::test_put()
{
  http::server s(7779);
  s.add_routing_middleware();

  s.on_put("/test/{name}", [](const http::request &req) {
    return http::response::ok("<h1>" + req.body() + " " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
  });

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (400));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  http::client c("localhost:7779");
  auto resp = c.put("/test/world", "hello");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  std::this_thread::sleep_for(std::chrono::seconds (1));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void HttpServerTest::test_delete()
{
  http::server s(7779);
  s.add_routing_middleware();

  s.on_remove("/test/{name}", [](const http::request &req) {
    return http::response::ok("<h1>hello " + req.path_params().at("name") + "</h1>", http::mime_types::TYPE_TEXT_HTML);
  });

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (400));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  http::client c("localhost:7779");
  auto resp = c.remove("/test/world");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  std::this_thread::sleep_for(std::chrono::seconds (1));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}
