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

http::response dummy(const http::request &, const http::route_path::t_path_param_map &)
{
  return http::response();
}

void RouteEngineTest::test_routes()
{
  http::routing_engine re;

  re.add("/api/user", http::http::GET, dummy);
  // sentinel path param id
  re.add("/api/user/{id}", http::http::GET, dummy);
  // sentinel path param id only numbers
  re.add("/api/role/{id: \\d+}", http::http::GET, dummy);
  // path param type
  re.add("/api/find/{type}/all", http::http::GET, dummy);
  // path param type and id only numbers
  re.add("/api/find/{type}/{id: \\d+}", http::http::GET, [](const http::request &, const http::route_path::t_path_param_map &) { return http::response(); });

//  re.dump(std::cout);

  http::route_path::t_path_param_map path_params;
  // valid routes
  // /api/user
  auto rep = re.find("/api/user", http::http::GET, path_params);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_EQUAL("user", (*rep)->endpoint_name());
  UNIT_ASSERT_TRUE(path_params.empty());

  // /api/user/a1
  rep = re.find("/api/user/a1", http::http::GET, path_params);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_EQUAL("{id}", (*rep)->endpoint_name());
  UNIT_ASSERT_FALSE(path_params.empty());
  UNIT_ASSERT_EQUAL(1UL, path_params.size());
  UNIT_ASSERT_TRUE(path_params.find("id") != path_params.end());
  UNIT_ASSERT_EQUAL("a1", path_params["id"]);

  // /api/role/1234
  rep = re.find("/api/role/1234", http::http::GET, path_params);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_EQUAL("{id: \\d+}", (*rep)->endpoint_name());
  UNIT_ASSERT_FALSE(path_params.empty());
  UNIT_ASSERT_EQUAL(1UL, path_params.size());
  UNIT_ASSERT_TRUE(path_params.find("id") != path_params.end());
  UNIT_ASSERT_EQUAL("1234", path_params["id"]);

  // /api/find/post/all
  rep = re.find("/api/find/post/all", http::http::GET, path_params);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_EQUAL("all", (*rep)->endpoint_name());
  UNIT_ASSERT_FALSE(path_params.empty());
  UNIT_ASSERT_EQUAL(1UL, path_params.size());
  UNIT_ASSERT_TRUE(path_params.find("type") != path_params.end());
  UNIT_ASSERT_EQUAL("post", path_params["type"]);

  // /api/find/post/17
  rep = re.find("/api/find/post/17", http::http::GET, path_params);
  UNIT_ASSERT_TRUE(re.valid(rep));
  UNIT_ASSERT_EQUAL("{id: \\d+}", (*rep)->endpoint_name());
  UNIT_ASSERT_FALSE(path_params.empty());
  UNIT_ASSERT_EQUAL(2UL, path_params.size());
  UNIT_ASSERT_TRUE(path_params.find("type") != path_params.end());
  UNIT_ASSERT_EQUAL("post", path_params["type"]);
  UNIT_ASSERT_TRUE(path_params.find("id") != path_params.end());
  UNIT_ASSERT_EQUAL("17", path_params["id"]);

  // invalid routes
  // /api/users
  rep = re.find("/api/users", http::http::GET, path_params);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/user/a1/team
  rep = re.find("/api/user/a1/team", http::http::GET, path_params);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/role/a1234
  rep = re.find("/api/role/a1234", http::http::GET, path_params);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/find/post/first
  rep = re.find("/api/find/post/first", http::http::GET, path_params);
  UNIT_ASSERT_FALSE(re.valid(rep));

  // /api/find/post/a17
  rep = re.find("/api/find/post/a17", http::http::GET, path_params);
  UNIT_ASSERT_FALSE(re.valid(rep));
}
