#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("Drop intermediate creates object-specific queries", "[query][drop]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};

  const auto drop_table = drop().table(customers);
  const auto drop_sequence = drop().sequence("customer_id_seq");
  const auto drop_schema = drop().schema("archive");

  REQUIRE(drop_table.str(dialect) == R"(DROP TABLE "customers")");
  REQUIRE(drop_sequence.str(dialect) == R"(DROP SEQUENCE "customer_id_seq")");
  REQUIRE(drop_schema.str(dialect) == R"(DROP SCHEMA "archive")");
  REQUIRE(drop_table.compile(dialect).command == query_command::DropTable);
  REQUIRE(drop_sequence.compile(dialect).command == query_command::DropSequence);
  REQUIRE(drop_schema.compile(dialect).command == query_command::DropSchema);
}

TEST_CASE("Drop intermediate keeps earlier states reusable", "[query][drop]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto base = drop();
  const auto drop_table = base.table(customers);
  const auto drop_sequence = base.sequence("customer_id_seq");
  const auto drop_schema = base.schema("archive");

  REQUIRE(drop_table.str(dialect) == R"(DROP TABLE "customers")");
  REQUIRE(drop_sequence.str(dialect) == R"(DROP SEQUENCE "customer_id_seq")");
  REQUIRE(drop_schema.str(dialect) == R"(DROP SCHEMA "archive")");
}
