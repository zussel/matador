#include <catch2/catch_test_macros.hpp>

#include "matador/query/dialect.hpp"
#include "matador/query/query.hpp"
#include "matador/query/table.hpp"

#include <cstdint>

namespace matador::query {
class dialect_builder {
public:
  static dialect make_default() {
    return {};
  }
};
}

using namespace matador::query;

TEST_CASE("Insert intermediate keeps target states reusable", "[query][insert]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const table orders{"orders"};
  const auto base = insert();
  const auto customer_target = base.into(customers, {"id"_col});
  const auto order_target = base.into(orders, {"number"_col});

  const auto customer_insert = customer_target.values({matador::utils::database_type{std::int32_t{1}}});
  const auto order_insert = order_target.values({matador::utils::database_type{std::int32_t{2}}});

  REQUIRE(customer_insert.str(dialect) == R"(INSERT INTO "customers" ("id") VALUES (1))");
  REQUIRE(order_insert.str(dialect) == R"(INSERT INTO "orders" ("number") VALUES (2))");
  REQUIRE(customer_insert.compile(dialect).command == query_command::Insert);
}

TEST_CASE("Insert intermediate keeps values states reusable", "[query][insert]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto target = insert().into(customers, {"id"_col});
  const auto first_insert = target.values({matador::utils::database_type{std::int32_t{1}}});
  const auto second_insert = target.values({matador::utils::database_type{std::int32_t{2}}});
  const auto returning_insert = first_insert.returning("id"_col);

  REQUIRE(first_insert.str(dialect) == R"(INSERT INTO "customers" ("id") VALUES (1))");
  REQUIRE(second_insert.str(dialect) == R"(INSERT INTO "customers" ("id") VALUES (2))");
  REQUIRE(returning_insert.str(dialect) == R"(INSERT INTO "customers" ("id") VALUES (1) RETURNING "id")");
}
