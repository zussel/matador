#include "catch2/catch_test_macros.hpp"

#include "matador/query/runtime/connection.hpp"

#include "connection.hpp"

using namespace matador::query;

TEST_CASE("Create connection test", "[connection]") {
  const connection c(matador::test::connection::dns);
  auto res = c.is_open();
  REQUIRE(res);
  REQUIRE(!*res);

  auto result = c.open();
  REQUIRE(result.is_ok());
  res = c.is_open();
  REQUIRE(res);
  REQUIRE(*res);

  result = c.close();
  REQUIRE(result.is_ok());
  res = c.is_open();
  REQUIRE(res);
  REQUIRE(!*res);
}
