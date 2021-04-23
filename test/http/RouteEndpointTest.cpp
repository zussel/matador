#include "RouteEndpointTest.hpp"

#include "matador/utils/string.hpp"

#include "matador/http/routing_engine.hpp"
#include "matador/http/response.hpp"

#include <regex>
#include <unordered_map>

using namespace matador;

RouteEndpointTest::RouteEndpointTest()
  : matador::unit_test("route_endpoint", "route endpoint test")
{
  add_test("route_endpoint", [this] { test_route_endpoint(); }, "test route endpoint");
}

void RouteEndpointTest::test_route_endpoint()
{
  check_route("/", R"(\/)", "/", 0);
  check_route("/api/v1/user", R"(\/api\/v1\/user)", "/api/v1/user", 0);
  check_route("/api/v1/{type}/list", R"(\/api\/v1\/(\w+)\/list)", "/api/v1/role/list", 1);
  check_route("/api/v1/user/{id: \\d+}", R"(\/api\/v1\/user\/(\d+))", "/api/v1/user/17", 1);
  check_route("/app/*.*", R"(\/app\/([^?]*))", "/app/index.html", 0);
  check_route("/app/*.*", R"(\/app\/([^?]*))", "/app/", 0);
}

void RouteEndpointTest::check_route(
  const std::string &route_spec,
  const std::string &expected_regex_result,
  const std::string &valid_route,
  size_t num_path_params
) {

  try {
    auto rep = http::create_route_endpoint(route_spec, http::http::method_t::GET, [](const http::request &) { return http::response(); });

    UNIT_ASSERT_EQUAL(expected_regex_result, rep->path_regex());
    UNIT_ASSERT_EQUAL(num_path_params, rep->param_map().size());

    std::regex r(rep->path_regex());

    std::smatch what;

    UNIT_ASSERT_TRUE(std::regex_match(valid_route, what, r));

  } catch (std::exception &ex) {
    UNIT_FAIL(ex.what());
  }
}
