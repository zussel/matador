#include "RouteEngineTest.hpp"

#include "matador/http/route_engine.hpp"
#include "matador/http/response.hpp"

using namespace matador;

RouteEngineTest::RouteEngineTest()
  : matador::unit_test("route_engine", "route engine test")
{
  add_test("routes", [this] { test_routes(); }, "test route engine");
}

void RouteEngineTest::test_routes()
{
  http::route_engine re;

  re.add("/api/user", http::http::GET, [](const http::request &req) { return http::response(); });
  // sentinel path param id
  re.add("/api/user/{id}", http::http::GET, [](const http::request &req) { return http::response(); });
  // sentinel path param id only numbers
  re.add("/api/role/{id: \\d+}", http::http::GET, [](const http::request &req) { return http::response(); });
  // path param type
  re.add("/api/find/{type}/all", http::http::GET, [](const http::request &req) { return http::response(); });
  // path param type and id only numbers
  re.add("/api/find/{type}/{id \\+}", http::http::GET, [](const http::request &req) { return http::response(); });

//  re.dump(std::cout);

  // valid routes
  // /api/user
  auto rep = re.find("/api/user", http::http::GET);
  UNIT_ASSERT_EQUAL("user", rep.endpoint_name());

  UNIT_ASSERT_TRUE(http::is_path_param("{type}"));
  UNIT_ASSERT_TRUE(http::is_path_param("{id: \\d+}"));
  UNIT_ASSERT_FALSE(http::is_path_param("user"));
  // /api/user/a1
  // /api/role/1234
  // /api/find/post/all
  // /api/find/post/17

  // invalid routes
  // /api/users
  // /api/user/a1/team
  // /api/role/a1234
  // /api/find/post/first
  // /api/find/post/a17

}
