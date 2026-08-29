#include <catch2/catch_test_macros.hpp>

#include "matador/query/column.hpp"
#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/table.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace matador::query;
using namespace matador::utils;

TEST_CASE("Column: plain columns expose names, aliases, and types", "[query][column]") {
  const column default_column;
  REQUIRE(default_column.is_plain_column());
  REQUIRE(default_column.name().empty());
  REQUIRE(default_column.column_name().empty());
  REQUIRE(default_column.alias().empty());
  REQUIRE(default_column.result_name().empty());
  REQUIRE(default_column.type() == basic_type::Unknown);
  REQUIRE_FALSE(default_column.is_function());
  REQUIRE_FALSE(default_column.is_expression());
  REQUIRE(default_column.function() == query_functions::None);
  REQUIRE(default_column.expression() == nullptr);

  const column id{"id", "customer_id"};
  REQUIRE(id.name() == "id");
  REQUIRE(id.column_name() == "id");
  REQUIRE(id.canonical_name() == "id");
  REQUIRE(id.alias() == "customer_id");
  REQUIRE(id.result_name() == "customer_id");
  REQUIRE(id.has_alias());
  REQUIRE(std::string(id) == "id");
}

TEST_CASE("Column: table ownership qualifies the name", "[query][column]") {
  const table customers{"customers"};
  column id{&customers, "id", basic_type::Int32};

  REQUIRE(id.table() == &customers);
  REQUIRE(id.name() == "customers.id");
  REQUIRE(id.canonical_name() == "customers.id");
  REQUIRE(id.column_name() == "id");
  REQUIRE(id.result_name() == "id");
  REQUIRE(id.type() == basic_type::Int32);

  table archived_customers{"archived_customers"};
  id.table(&archived_customers);
  REQUIRE(id.table() == &archived_customers);
  REQUIRE(id.name() == "archived_customers.id");

  archived_customers = table{"former_customers"};
  REQUIRE(id.name() == "former_customers.id");

  const table order{"order", {column{"number"}}};
  REQUIRE(order.columns().front().table() == &order);
  REQUIRE(order.columns().front().name() == "order.number");
}

TEST_CASE("Column: as preserves its value and replaces its alias", "[query][column]") {
  const table customers{"customers"};
  const column original{&customers, "id", basic_type::Int64};
  const column aliased = original.as("customer_id");

  REQUIRE(original.alias().empty());
  REQUIRE(aliased.alias() == "customer_id");
  REQUIRE(aliased.has_alias());
  REQUIRE(aliased.result_name() == "customer_id");
  REQUIRE(aliased.table() == &customers);
  REQUIRE(aliased.name() == "customers.id");
  REQUIRE(aliased.type() == basic_type::Int64);
}

TEST_CASE("Column: equality includes table-bound column identity and aliases", "[query][column]") {
  const table customers{"customers"};
  const table orders{"orders"};

  const column customer_id{&customers, "id"};
  REQUIRE(customer_id.equals(column{&customers, "id"}));
  REQUIRE_FALSE(customer_id.equals(column{&customers, "name"}));
  REQUIRE_FALSE(customer_id.equals(column{&orders, "id"}));
  REQUIRE_FALSE(customer_id.equals(customer_id.as("customer_id")));
}

TEST_CASE("Column: equality distinguishes every active value", "[query][column]") {
  const column id{"id"};
  const column name{"name"};
  REQUIRE(id.equals(column{"id"}));
  REQUIRE_FALSE(id.equals(name));

  const column count_id{query_functions::Count, "id"};
  const column sum_id{query_functions::Sum, "id"};
  REQUIRE(count_id.equals(column{query_functions::Count, "id"}));
  REQUIRE_FALSE(count_id.equals(sum_id));
  REQUIRE_FALSE(id.equals(count_id));

  const auto expression = std::make_shared<placeholder_expression>();
  const column first_expression{expression};
  const column same_expression{expression};
  const column other_expression{std::make_shared<placeholder_expression>()};
  REQUIRE(first_expression.equals(same_expression));
  REQUIRE_FALSE(first_expression.equals(other_expression));
  REQUIRE_FALSE(first_expression.equals(first_expression.as("placeholder")));
}

TEST_CASE("Column: function and expression columns retain their active value", "[query][column]") {
  const column count{query_functions::Count, "id"};
  REQUIRE(count.is_function());
  REQUIRE_FALSE(count.is_expression());
  REQUIRE(count.function() == query_functions::Count);
  REQUIRE(count.column_name() == "id");
  REQUIRE(count.name() == "id");
  REQUIRE(count.type() == basic_type::Unknown);
  REQUIRE(count.expression() == nullptr);

  const auto expression = std::make_shared<placeholder_expression>();
  const column calculated{expression};
  const column aliased = calculated.as("calculated_id");
  REQUIRE_FALSE(calculated.is_function());
  REQUIRE(calculated.is_expression());
  REQUIRE(calculated.expression() == expression);
  REQUIRE(calculated.function() == query_functions::None);
  REQUIRE(calculated.name().empty());
  REQUIRE(calculated.type() == basic_type::Unknown);
  REQUIRE(aliased.expression() == expression);
  REQUIRE(aliased.alias() == "calculated_id");
  REQUIRE(aliased.result_name() == "calculated_id");

  column table_expression{expression};
  const table customers{"customers"};
  REQUIRE_THROWS_AS(table_expression.table(&customers), std::logic_error);

  const std::vector<column> schema{table_expression};
  REQUIRE_THROWS_AS((table{"calculated", schema}), std::logic_error);
}

TEST_CASE("Column: literal accepts only unqualified column names", "[query][column]") {
  const column bare = "id"_col;

  REQUIRE(bare.column_name() == "id");
  REQUIRE(bare.table() == nullptr);
  REQUIRE_THROWS_AS("customers.id"_col, std::invalid_argument);
  REQUIRE_THROWS_AS("schema.customers.id"_col, std::invalid_argument);
}
