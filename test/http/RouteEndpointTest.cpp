#include "RouteEndpointTest.hpp"

#include "matador/utils/string.hpp"

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
  check_route("/api/v1/user", 4, R"(\/api\/v1\/user)", "/api/v1/user", 0);
  check_route("/api/v1/{type}/list", 5, R"(\/api\/v1\/(\w+)\/list)", "/api/v1/role/list", 1);
  check_route("/api/v1/user/{id: \\d+}", 5, R"(\/api\/v1\/user\/(\d+))", "/api/v1/user/17", 1);
  check_route("/app/*.*", 3, R"(\/app\/(.*))", "/app/index.html", 0);
  check_route("/app/*.*", 3, R"(\/app\/(.*))", "/app/", 0);
}

void RouteEndpointTest::check_route(
  const std::string &route_spec,
  size_t numparts,
  const std::string &expected_regex_result,
  const std::string &valid_route,
  size_t num_path_params
) {
  std::regex route_regex(R"((\*\.\*)|\{(\w+)(:\s*(.*))?\}|([a-zA-Z0-9-_]+))");

  std::smatch what;

  std::unordered_map<std::string, size_t> path_param_to_index_map;
  std::vector<std::string> parts;

  UNIT_ASSERT_EQUAL(numparts, split(route_spec, '/', parts));

  std::string result_regex;

  size_t index = 0;
  for (const auto &part : parts) {
    if (part.empty()) {
      continue;
    }
    UNIT_ASSERT_TRUE(std::regex_match(part, what, route_regex));

    result_regex += R"(\/)";
    if (what[1].matched) {
      // static files
      result_regex += "(.*)";
    } else if (what[5].matched) {
      // plain path element
      result_regex += what[5].str();
    } else if (what[4].matched) {
      // path param with regex
      result_regex += "(" + what[4].str() + ")";
      // what[2] is path name
      path_param_to_index_map.insert(std::make_pair(what[2].str(), index++));
    } else if (what[2].matched) {
      // path param
      result_regex += R"((\w+))";
      path_param_to_index_map.insert(std::make_pair(what[2].str(), index++));
    } else {
      UNIT_FAIL("invalid regex match");
    }
  }

  UNIT_ASSERT_EQUAL(expected_regex_result, result_regex);
  UNIT_ASSERT_EQUAL(num_path_params, path_param_to_index_map.size());

  std::regex r(result_regex);

  UNIT_ASSERT_TRUE(std::regex_match(valid_route, what, r));

}
