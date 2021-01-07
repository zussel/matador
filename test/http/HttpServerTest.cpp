#include "HttpServerTest.hpp"

HttpServerTest::HttpServerTest()
  : matador::unit_test("http_server", "http server test")
{
  add_test("get", [this]() { test_get(); }, "http server get test");
}

void HttpServerTest::test_get()
{

}
