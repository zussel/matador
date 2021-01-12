#include "HttpServerTest.hpp"

#include "../net/NetUtils.hpp"

#include "matador/http/http_server.hpp"
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
}

void HttpServerTest::test_shutdown()
{
  http::server s(7777);

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void HttpServerTest::test_get()
{
//  matador::default_min_log_level(log_level::LVL_DEBUG);
//  matador::add_log_sink(matador::create_stdout_sink());

  http::server s(7778);

  s.on_get("/test", [](const http::request &req) {
    return http::response::no_content();
  });

  ThreadWrapper<http::server> wrapper(s);

  wrapper.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  // send and verify received data
  tcp::socket client;

  int ret = client.open(tcp::v4());
  UNIT_ASSERT_FALSE(ret < 0);
  auto srv = tcp::peer(address::v4::loopback(), 7778);
  ret = client.connect(srv);
  UNIT_ASSERT_FALSE(ret < 0);
  buffer data;
  data.append("GET /test HTTP/1.1\r\nHost: localhost:7778\r\n\r\n");
  ssize_t len = client.send(data);
  UNIT_ASSERT_EQUAL(44L, len);
  data.clear();

  std::string resp;
  do {
    len = client.receive(data);

    if (len < 0) {
      UNIT_FAIL("error on receive");
    } else if (len == 0) {
      break;
    }
    resp.append(data.data(), len);
    data.clear();
  } while (len > 0);

//  std::cout << "response: " << resp << "\n";

  UNIT_ASSERT_EQUAL(107UL, resp.size());
  client.close();

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}
