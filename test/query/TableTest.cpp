#include <catch2/catch_test_macros.hpp>

#include "matador/query/constraint.hpp"
#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/table.hpp"

#include <cstdint>
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

TEST_CASE("Table: constraints retain their table and column identity", "[query][table]") {
  column_constraints id_constraints{column_constraint::PrimaryKey};
  id_constraints.set(column_constraint::NotNull);
  const table orders{
    "orders",
    {column{"id", basic_type::Int64, id_constraints},
     column{"customer_id", basic_type::Int64, column_constraint::ForeignKey},
     column{"reference", basic_type::Varchar, column_constraint::Unique},
     column{"created_at", basic_type::DateTime, column_constraint::Index},
     column{"sequence", basic_type::Int64, column_constraint::Identity},
     column{"status", basic_type::Varchar, column_constraint::Default}}
  };

  const auto constraints = orders.constraints();
  REQUIRE(constraints.size() == 7);

  const auto& primary_key = constraints[0];
  REQUIRE(primary_key.kind() == column_constraint::PrimaryKey);
  REQUIRE(primary_key.table() == orders);
  REQUIRE(&primary_key.column() == &orders.columns()[0]);
  REQUIRE(primary_key.column_index() == 0);
  REQUIRE(primary_key.is_primary_key_constraint());
  REQUIRE_FALSE(primary_key.is_foreign_key_constraint());
  REQUIRE_FALSE(primary_key.is_not_null_constraint());

  const auto& not_null = constraints[1];
  REQUIRE(not_null.kind() == column_constraint::NotNull);
  REQUIRE(not_null.is_not_null_constraint());
  REQUIRE(not_null.column_index() == 0);

  const auto& foreign_key = constraints[2];
  REQUIRE(foreign_key.kind() == column_constraint::ForeignKey);
  REQUIRE(foreign_key.is_foreign_key_constraint());
  REQUIRE(foreign_key.column_index() == 1);

  const auto& unique = constraints[3];
  REQUIRE(unique.kind() == column_constraint::Unique);
  REQUIRE(unique.is_unique_constraint());
  REQUIRE(unique.column_index() == 2);

  const auto& index = constraints[4];
  REQUIRE(index.kind() == column_constraint::Index);
  REQUIRE(index.is_index_constraint());
  REQUIRE(index.column_index() == 3);

  const auto& identity = constraints[5];
  REQUIRE(identity.kind() == column_constraint::Identity);
  REQUIRE(identity.is_identity_constraint());
  REQUIRE(identity.column_index() == 4);

  const auto& default_value = constraints[6];
  REQUIRE(default_value.kind() == column_constraint::Default);
  REQUIRE(default_value.is_default_constraint());
  REQUIRE(default_value.column_index() == 5);

  const table copied{orders};
  const table aliased = orders.as("o");
  table move_source{orders};
  const table moved{std::move(move_source)};
  for (const auto* derived : {&copied, &aliased, &moved}) {
    const auto derived_constraints = derived->constraints();
    REQUIRE(derived_constraints.size() == constraints.size());
    REQUIRE(&derived_constraints.front().table() == derived);
    REQUIRE(&derived_constraints.front().column() == &derived->columns().front());
  }
}

TEST_CASE("Constraint: validates its referenced column and kind", "[query][constraint]") {
  const table orders{"orders", {column{"id", basic_type::Int64}}};

  REQUIRE_THROWS_AS((constraint{orders, 1, column_constraint::Unique}), std::out_of_range);
  REQUIRE_THROWS_AS((constraint{orders, 0, column_constraint::None}), std::invalid_argument);
  REQUIRE_THROWS_AS((constraint{orders, 0, column_constraint::Unique}), std::invalid_argument);
  REQUIRE_THROWS_AS((constraint{
                      orders, 0, static_cast<column_constraint>(
                        static_cast<uint8_t>(column_constraint::Unique) |
                        static_cast<uint8_t>(column_constraint::NotNull))}),
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
