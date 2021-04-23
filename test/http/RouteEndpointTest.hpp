#ifndef MATADOR_ROUTEENDPOINTTEST_HPP
#define MATADOR_ROUTEENDPOINTTEST_HPP

#include "matador/unit/unit_test.hpp"

class RouteEndpointTest : public matador::unit_test
{
public:
  RouteEndpointTest();

  void test_route_endpoint();

private:
  void check_route(
    const std::string &route_spec,
    const std::string &expected_regex_result, const std::string &valid_route,
    size_t num_path_params
  );
};


#endif //MATADOR_ROUTEENDPOINTTEST_HPP
