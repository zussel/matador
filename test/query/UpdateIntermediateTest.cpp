#include <catch2/catch_test_macros.hpp>

#include "matador/query/criteria.hpp"
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

TEST_CASE("Update intermediate keeps assignment states reusable", "[query][update]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto base = update(customers);
  const auto status_update = base.set("status"_col, column_expression{std::int32_t{1}});
  const auto priority_update = base.set("priority"_col, column_expression{std::int32_t{2}});
  const auto combined_update = status_update.set("priority"_col, column_expression{std::int32_t{2}});

  REQUIRE(status_update.str(dialect) == R"(UPDATE "customers" SET "status"=1)");
  REQUIRE(priority_update.str(dialect) == R"(UPDATE "customers" SET "priority"=2)");
  REQUIRE(combined_update.str(dialect) == R"(UPDATE "customers" SET "status"=1, "priority"=2)");
  REQUIRE(combined_update.compile(dialect).command == query_command::Update);
}

TEST_CASE("Update intermediate preserves unfiltered states", "[query][update]") {
  const dialect dialect = dialect_builder::make_default();
  const table customers{"customers"};
  const auto unfiltered_update = update(customers).set("status"_col, column_expression{std::int32_t{1}});
  const auto filtered_update = unfiltered_update.where("id"_col == 42);

  REQUIRE(unfiltered_update.str(dialect) == R"(UPDATE "customers" SET "status"=1)");
  REQUIRE(filtered_update.str(dialect) == R"(UPDATE "customers" SET "status"=1 WHERE "id" = 42)");
}
