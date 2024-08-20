#include "catch2/catch_test_macros.hpp"

#include "matador/sql/connection.hpp"

#include "connection.hpp"

using namespace matador::sql;

TEST_CASE("Create connection test", "[connection]") {
  const connection c(matador::test::connection::dns);
  REQUIRE(!c.is_open());

  c.open();
  REQUIRE(c.is_open());

  c.close();
  REQUIRE(!c.is_open());
}
