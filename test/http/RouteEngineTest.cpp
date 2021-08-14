#include "RouteEngineTest.hpp"

#include "matador/http/routing_engine.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"
#include "matador/http/request_parser.hpp"

using namespace matador;

RouteEngineTest::RouteEngineTest()
  : matador::unit_test("route_engine", "route engine test")
{
  add_test("routes", [this] { test_routes(); }, "test route engine");
}

http::response dummy(const http::request &)
{
  return http::response();
}

void RouteEngineTest::test_routes()
{
  http::routing_engine re;

  re.add("/", http::http::GET, dummy);
  re.add("/api/user", http::http::GET, dummy);
  // sentinel path param id
  re.add("/api/user/{id}", http::http::GET, dummy);
  // sentinel path param id only numbers
  re.add("/api/role/{id: \\d+}", http::http::GET, dummy);
  // path param type
  re.add("/api/find/{type}/all", http::http::GET, dummy);
  // path param type and id only numbers
  re.add("/api/find/{type}/{id: \\d+}", http::http::GET, [](const http::request &) { return http::response(); });

  // valid routes
  // /api/user
  http::request req;
  http::request_parser parser;

  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/user HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  auto rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_TRUE(req.path_params().empty());

  // /api/user/a1
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/user/a1 HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_FALSE(req.path_params().empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_params().size());
  UNIT_ASSERT_TRUE(req.path_params().find("id") != req.path_params().end());
  UNIT_ASSERT_EQUAL("a1", req.path_params().at("id"));

  // /api/role/1234
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/role/1234 HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_FALSE(req.path_params().empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_params().size());
  UNIT_ASSERT_TRUE(req.path_params().find("id") != req.path_params().end());
  UNIT_ASSERT_EQUAL("1234", req.path_params().at("id"));

  // /
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET / HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_TRUE(req.path_params().empty());

  // /api/find/post/all
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/find/post/all HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_FALSE(req.path_params().empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_params().size());
  UNIT_ASSERT_TRUE(req.path_params().find("type") != req.path_params().end());
  UNIT_ASSERT_EQUAL("post", req.path_params().at("type"));

  // /api/find/post/17
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/find/post/17 HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_FALSE(req.path_params().empty());
  UNIT_ASSERT_EQUAL(2UL, req.path_params().size());
  UNIT_ASSERT_TRUE(req.path_params().find("type") != req.path_params().end());
  UNIT_ASSERT_EQUAL("post", req.path_params().at("type"));
  UNIT_ASSERT_TRUE(req.path_params().find("id") != req.path_params().end());
  UNIT_ASSERT_EQUAL("17", req.path_params().at("id"));

  // invalid routes
  // /api/users
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/users HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/user/a1/team
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/user/a1/team HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/role/a1234
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/role/a1234 HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/find/post/first
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/find/post/first HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/find/post/a17
  req = http::request();
  UNIT_ASSERT_EQUAL(http::request_parser::FINISH, parser.parse("GET /api/find/post/a17 HTTP/1.1\r\nHost: de.wikipedia.org\r\n\r\n", req));
  rep = re.match(req);
  UNIT_ASSERT_FALSE(re.valid(rep));
}
