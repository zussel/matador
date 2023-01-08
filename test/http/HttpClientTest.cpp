#include "HttpClientTest.hpp"

#include "HttpTestServer.hpp"

#include "matador/http/http_client.hpp"

#include "../net/NetUtils.hpp"

using namespace matador;
using namespace ::detail;

HttpClientTest::HttpClientTest()
  : matador::unit_test("http_client", "http client test")
{
  add_test("get", [this]() { test_get(); }, "http client get test");
  add_test("post", [this]() { test_post(); }, "http client post test");
  add_test("put", [this]() { test_put(); }, "http client put test");
  add_test("delete", [this]() { test_delete(); }, "http client delete test");
}

void HttpClientTest::test_get() {
  HttpTestServer server(8123);

  utils::ThreadRunner runner([&server]() {
    server.run();
    }, [&server](){
    server.shutdown();
  }
  );

  UNIT_ASSERT_TRUE(utils::wait_until_running(server));

  http::client c("localhost:8123");
  auto resp = c.get("/test/world");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server));
}

void HttpClientTest::test_post() {
  HttpTestServer server(8123);

  utils::ThreadRunner runner([&server]() {
                               server.run();
                             }, [&server](){
                               server.shutdown();
  }
  );

  UNIT_ASSERT_TRUE(utils::wait_until_running(server));

  http::client c("localhost:8123");
  auto resp = c.post("/test/world", "hello");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server));
}

void HttpClientTest::test_put() {
  HttpTestServer server(8123);

  utils::ThreadRunner runner([&server]() {
                               server.run();
                             }, [&server](){
                               server.shutdown();
  }
  );

  UNIT_ASSERT_TRUE(utils::wait_until_running(server));

  http::client c("localhost:8123");
  auto resp = c.put("/test/world", "hello");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server));
}

void HttpClientTest::test_delete() {
  HttpTestServer server(8123);

  utils::ThreadRunner runner([&server]() {
                               server.run();
                             }, [&server](){
                               server.shutdown();
  }
  );

  UNIT_ASSERT_TRUE(utils::wait_until_running(server));

  http::client c("localhost:8123");
  auto resp = c.remove("/test/world");

  UNIT_ASSERT_EQUAL("<h1>hello world</h1>", resp.body());
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server));
}
