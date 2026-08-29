#include <catch2/catch_test_macros.hpp>

#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/table.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace matador::query;
using namespace matador::utils;

namespace {

class typed_orders final : public typed_query_table<typed_orders> {
public:
  using typed_query_table::typed_query_table;

  explicit typed_orders(table source)
  : typed_query_table(std::move(source)) {}
};

} // namespace

TEST_CASE("Table: schema, identity, and aliasing are distinct", "[query][table]") {
  const table orders{"sales", "orders",
                     {column{"id", basic_type::Int64, column_constraint::PrimaryKey},
                      column{"total", basic_type::Double, column_constraint::NotNull}}};

  REQUIRE(orders.schema_name() == "sales");
  REQUIRE(orders.table_name() == "orders");
  REQUIRE(orders.qualified_name() == "sales.orders");
  REQUIRE(orders.name() == "sales.orders");
  REQUIRE_FALSE(orders.has_alias());
  REQUIRE(orders.columns().front().name() == "sales.orders.id");

  const table archived = orders.as("archived_orders");
  REQUIRE(archived.schema_name() == "sales");
  REQUIRE(archived.table_name() == "orders");
  REQUIRE(archived.qualified_name() == "sales.orders");
  REQUIRE(archived.name() == "archived_orders");
  REQUIRE(archived.has_alias());
  REQUIRE(archived.columns().front().name() == "archived_orders.id");
  REQUIRE(archived == orders.as("archived_orders"));
  REQUIRE_FALSE(archived == orders);
  REQUIRE_FALSE(orders == table{"archive", "orders", {}});
}

TEST_CASE("Table: primary-key metadata is validated and preserved", "[query][table]") {
  const table orders{"orders",
                     {column{"id", basic_type::Int64, column_constraint::PrimaryKey},
                      column{"total", basic_type::Double, column_constraint::NotNull}}};

  REQUIRE(orders.has_primary_key());
  REQUIRE(orders.primary_key_column() == &orders.columns().front());
  REQUIRE(orders.primary_key_column()->column_name() == "id");
  REQUIRE(orders.primary_key_column()->is_primary_key());
  REQUIRE(orders.primary_key_column()->constraints().has(column_constraint::PrimaryKey));

  const table copied{orders};
  REQUIRE(copied.primary_key_column() == &copied.columns().front());
  REQUIRE(copied.primary_key_column()->table() == &copied);

  const table aliased = orders.as("o");
  REQUIRE(aliased.primary_key_column() == &aliased.columns().front());
  REQUIRE(aliased.primary_key_column()->table() == &aliased);

  table move_source{orders};
  const table moved{std::move(move_source)};
  REQUIRE(moved.has_primary_key());
  REQUIRE_FALSE(move_source.has_primary_key());
  REQUIRE(move_source.primary_key_column() == nullptr);

  REQUIRE_FALSE(table{"audit", {column{"event_id"}}}.has_primary_key());
  REQUIRE(table{"audit", {column{"event_id"}}}.primary_key_column() == nullptr);

  REQUIRE_THROWS_AS(
    (table{"invalid",
           {column{"first", basic_type::Int32, column_constraint::PrimaryKey},
            column{"second", basic_type::Int32, column_constraint::PrimaryKey}}}),
    std::invalid_argument);
}

TEST_CASE("Table: lookup has nullable and throwing forms", "[query][table]") {
  const table orders{"orders", {column{"id"}, column{"total"}}};

  REQUIRE(orders["id"] == &orders.columns().front());
  REQUIRE(orders.find_column("total") == &orders.columns().back());
  REQUIRE(orders.column_by_name(orders, "id") == &orders.columns().front());
  REQUIRE(orders.at_column("total").column_name() == "total");
  REQUIRE(orders.column_ref_by_name(orders, "id").column_name() == "id");
  REQUIRE(orders["missing"] == nullptr);
  REQUIRE(orders.find_column("missing") == nullptr);
  REQUIRE_THROWS_AS(orders.at_column("missing"), std::invalid_argument);
  REQUIRE_THROWS_AS(orders.column_ref_by_name(orders, "missing"), std::invalid_argument);
}

TEST_CASE("Table: copy and move rebind column owners", "[query][table]") {
  const table original{"orders", {column{"id"}, column{"total"}}};
  table copied{original};
  REQUIRE(copied.columns().front().table() == &copied);
  REQUIRE(copied.columns().back().table() == &copied);

  table assigned{"placeholder"};
  assigned = original;
  REQUIRE(assigned.columns().front().table() == &assigned);
  REQUIRE(assigned.columns().back().table() == &assigned);

  table moved{std::move(copied)};
  REQUIRE(moved.columns().front().table() == &moved);
  REQUIRE(moved.columns().back().table() == &moved);

  table move_assigned{"placeholder"};
  move_assigned = std::move(assigned);
  REQUIRE(move_assigned.columns().front().table() == &move_assigned);
  REQUIRE(move_assigned.columns().back().table() == &move_assigned);
}

TEST_CASE("Table: schemas reject expression columns and null names", "[query][table]") {
  const column expression{std::make_shared<placeholder_expression>()};
  REQUIRE_THROWS_AS((table{"calculated", {expression}}), std::invalid_argument);
  REQUIRE_THROWS_AS(table{static_cast<const char*>(nullptr)}, std::invalid_argument);
  REQUIRE_THROWS_AS(column{static_cast<const char*>(nullptr)}, std::invalid_argument);
}

TEST_CASE("Table: typed aliases retain table state", "[query][table]") {
  const typed_orders orders{"sales", "orders", {column{"id", basic_type::Int64}}};

  const typed_orders alias = orders.as("o");
  REQUIRE(alias.schema_name() == "sales");
  REQUIRE(alias.table_name() == "orders");
  REQUIRE(alias.name() == "o");
  REQUIRE(alias.columns().size() == 1);
  REQUIRE(alias.columns().front().type() == basic_type::Int64);
  REQUIRE(alias.columns().front().table() == &alias);
}
