#include <catch2/catch_test_macros.hpp>

#include "matador/query/criteria.hpp"
#include "matador/query/dialect.hpp"
#include "matador/query/query.hpp"
#include "matador/query/table.hpp"

#include <stdexcept>

namespace matador::query {
class dialect_builder {
public:
  static dialect make_default() {
    return {};
  }
};
}

using namespace matador::query;

TEST_CASE("Select factory compiles requested columns and FROM tables", "[query][select]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};

  const auto query = select({"id"_col, "name"_col}).from(customers);

  REQUIRE(query.str(dialect) == R"(SELECT "id", "name" FROM "customers")");
  REQUIRE(query.compile(dialect).command == query_command::Select);
}

TEST_CASE("Select intermediate creates sequence queries", "[query][select]") {
  const dialect dialect = dialect_builder::make_default();

  SECTION("nextval") {
    const auto query = select().nextval("customer_id_seq");

    REQUIRE(query.str(dialect) == "SELECT NEXTVAL('customer_id_seq')");
    REQUIRE(query.compile(dialect).command == query_command::Select);
  }

  SECTION("currval") {
    const auto query = select().currval("customer_id_seq");

    REQUIRE(query.str(dialect) == "SELECT CURRVAL('customer_id_seq')");
    REQUIRE(query.compile(dialect).command == query_command::Select);
  }

  SECTION("sequence names escape string quotes") {
    const auto query = select().nextval("customer's_id_seq");

    REQUIRE(query.str(dialect) == "SELECT NEXTVAL('customer''s_id_seq')");
  }
}

TEST_CASE("Select intermediate keeps earlier states reusable", "[query][select]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto projection = select({"id"_col});
  const auto from_query = projection.from(customers);
  const auto sequence_query = projection.nextval("customer_id_seq");

  REQUIRE(from_query.str(dialect) == R"(SELECT "id" FROM "customers")");
  REQUIRE(sequence_query.str(dialect) == "SELECT NEXTVAL('customer_id_seq')");
}

TEST_CASE("Select fluent states preserve clauses across filtering and pagination", "[query][select]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto from_query = select({"id"_col}).from(customers);
  const auto filtered_query = from_query.where("id"_col == 42);
  const auto paged_query = filtered_query.order_by("id"_col).desc().limit(10).offset(5);

  REQUIRE(from_query.str(dialect) == R"(SELECT "id" FROM "customers")");
  REQUIRE(filtered_query.str(dialect) == R"(SELECT "id" FROM "customers" WHERE "id" = 42)");
  REQUIRE(paged_query.str(dialect) ==
    R"(SELECT "id" FROM "customers" WHERE "id" = 42 ORDER BY "id" DESC LIMIT 10 OFFSET 5)");
}

TEST_CASE("Select fluent states support joins and grouping", "[query][select]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const table orders{"orders"};

  const auto query = select({"id"_col})
    .from(customers)
    .join_left(orders)
    .on("customer_id"_col == "id"_col)
    .group_by("id"_col)
    .order_by("id"_col)
    .asc();

  REQUIRE(query.str(dialect) ==
    R"(SELECT "id" FROM "customers" LEFT JOIN "orders" ON "customer_id" = "id" GROUP BY "id" ORDER BY "id" ASC)");
}

TEST_CASE("Select intermediate rejects an empty FROM clause", "[query][select]") {
  const auto query = select({"id"_col});

  REQUIRE_THROWS_AS(query.from(), std::invalid_argument);
}
