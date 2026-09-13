#include <catch2/catch_test_macros.hpp>

#include "matador/query/criteria.hpp"
#include "matador/query/dialect.hpp"
#include "matador/query/query.hpp"
#include "matador/query/table.hpp"

namespace matador::query {
class dialect_builder {
public:
  static dialect make_default() {
    return {};
  }
};
}

using namespace matador::query;

TEST_CASE("Delete intermediate keeps source states reusable", "[query][delete]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const table orders{"orders"};
  const auto base = remove();
  const auto customer_delete = base.from(customers);
  const auto order_delete = base.from(orders);

  REQUIRE(customer_delete.str(dialect) == R"(DELETE FROM "customers")");
  REQUIRE(order_delete.str(dialect) == R"(DELETE FROM "orders")");
  REQUIRE(customer_delete.compile(dialect).command == query_command::Delete);
}

TEST_CASE("Delete intermediate preserves unfiltered states", "[query][delete]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto unfiltered_delete = remove().from(customers);
  const auto filtered_delete = unfiltered_delete.where("id"_col == 42);

  REQUIRE(unfiltered_delete.str(dialect) == R"(DELETE FROM "customers")");
  REQUIRE(filtered_delete.str(dialect) == R"(DELETE FROM "customers" WHERE "id" = 42)");
}
