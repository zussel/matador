#include "RouteEngineTest.hpp"

#include "matador/http/routing_engine.hpp"
#include "matador/http/response.hpp"
#include "matador/http/request.hpp"

using namespace matador;

RouteEngineTest::RouteEngineTest()
  : matador::unit_test("routing_engine", "route engine test")
{
  add_test("routes", [this] { test_routes(); }, "test route engine");
}

void RouteEngineTest::test_routes()
{
  http::routing_engine re;

  re.add("/api/user", http::http::GET, [](const http::request &) { return http::response(); });
  // sentinel path param id
  re.add("/api/user/{id}", http::http::GET, [](const http::request &) { return http::response(); });
  // sentinel path param id only numbers
  re.add("/api/role/{id: \\d+}", http::http::GET, [](const http::request &) { return http::response(); });
  // path param type
  re.add("/api/find/{type}/all", http::http::GET, [](const http::request &) { return http::response(); });
  // path param type and id only numbers
  re.add("/api/find/{type}/{id: \\d+}", http::http::GET, [](const http::request &) { return http::response(); });

//  re.dump(std::cout);

  http::request req;

  // valid routes
  // /api/user
  auto rep = re.find("/api/user", http::http::GET, req);
  UNIT_ASSERT_EQUAL("user", rep->endpoint_name());
  UNIT_ASSERT_TRUE(req.path_param_map_.empty());

  // /api/user/a1
  rep = re.find("/api/user/a1", http::http::GET, req);
  UNIT_ASSERT_EQUAL("{id}", rep->endpoint_name());
  UNIT_ASSERT_FALSE(req.path_param_map_.empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_param_map_.size());
  UNIT_ASSERT_TRUE(req.path_param_map_.find("id") != req.path_param_map_.end());
  UNIT_ASSERT_EQUAL("a1", req.path_param_map_["id"]);

  // /api/role/1234
  rep = re.find("/api/role/1234", http::http::GET, req);
  UNIT_ASSERT_EQUAL("{id: \\d+}", rep->endpoint_name());
  UNIT_ASSERT_FALSE(req.path_param_map_.empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_param_map_.size());
  UNIT_ASSERT_TRUE(req.path_param_map_.find("id") != req.path_param_map_.end());
  UNIT_ASSERT_EQUAL("1234", req.path_param_map_["id"]);

  // /api/find/post/all
  rep = re.find("/api/find/post/all", http::http::GET, req);
  UNIT_ASSERT_EQUAL("all", rep->endpoint_name());
  UNIT_ASSERT_FALSE(req.path_param_map_.empty());
  UNIT_ASSERT_EQUAL(1UL, req.path_param_map_.size());
  UNIT_ASSERT_TRUE(req.path_param_map_.find("type") != req.path_param_map_.end());
  UNIT_ASSERT_EQUAL("post", req.path_param_map_["type"]);

  // /api/find/post/17
  rep = re.find("/api/find/post/17", http::http::GET, req);
  UNIT_ASSERT_EQUAL("{id: \\d+}", rep->endpoint_name());
  UNIT_ASSERT_FALSE(req.path_param_map_.empty());
  UNIT_ASSERT_EQUAL(2UL, req.path_param_map_.size());
  UNIT_ASSERT_TRUE(req.path_param_map_.find("type") != req.path_param_map_.end());
  UNIT_ASSERT_EQUAL("post", req.path_param_map_["type"]);
  UNIT_ASSERT_TRUE(req.path_param_map_.find("id") != req.path_param_map_.end());
  UNIT_ASSERT_EQUAL("17", req.path_param_map_["id"]);

  // invalid routes
  // /api/users
  rep = re.find("/api/users", http::http::GET, req);
  UNIT_ASSERT_EQUAL(http::http::UNKNOWN, rep->method());

  // /api/user/a1/team
  rep = re.find("/api/user/a1/team", http::http::GET, req);
  UNIT_ASSERT_EQUAL(http::http::UNKNOWN, rep->method());

  // /api/role/a1234
  rep = re.find("/api/role/a1234", http::http::GET, req);
  UNIT_ASSERT_EQUAL(http::http::UNKNOWN, rep->method());

  // /api/find/post/first
  rep = re.find("/api/find/post/first", http::http::GET, req);
  UNIT_ASSERT_EQUAL(http::http::UNKNOWN, rep->method());

  // /api/find/post/a17
  rep = re.find("/api/find/post/a17", http::http::GET, req);
  UNIT_ASSERT_EQUAL(http::http::UNKNOWN, rep->method());
}
