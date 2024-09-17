#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column_generator.hpp"
#include "matador/sql/schema.hpp"
#include "matador/sql/table.hpp"

#include "models/order.hpp"
#include "models/product.hpp"
#include "models/book.hpp"
#include "models/author.hpp"

using namespace matador::sql;

TEST_CASE("Generate columns from object", "[column][generator]") {
  using namespace matador::test;
  schema s("main");
  s.attach<product>("product");

  auto columns = column_generator::generate<product>(s);

  const std::vector<std::string> expected_columns = {
    "product_name",
    "supplier_id",
    "category_id",
    "quantity_per_unit",
    "unit_price",
    "units_in_stock",
    "units_in_order",
    "reorder_level",
    "discontinued"
  };
  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());

  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i] == columns[i].name);
  }
}

TEST_CASE("Generate columns for object with has many relation", "[column][generator][relation]") {
  using namespace matador::test;
  schema s("main");
  s.attach<product>("product");
  s.attach<order_details>("order_details");
  s.attach<order>("order");

  const auto columns = column_generator::generate<order>(s);

  const table order_table{"order"};
  const table order_details_table{"order_details"};
  const std::vector<column> expected_columns = {
    { order_table, "order_id" },
    { order_table, "order_date" },
    { order_table, "required_date" },
    { order_table, "shipped_date" },
    { order_table, "ship_via" },
    { order_table, "freight" },
    { order_table, "ship_name" },
    { order_table, "ship_address" },
    { order_table, "ship_city" },
    { order_table, "ship_region" },
    { order_table, "ship_postal_code" },
    { order_table, "ship_country" },
    { order_details_table, "order_details_id" },
    { order_details_table, "order_id" },
    { order_details_table, "product_id" }
  };
  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());

  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(columns[i]));
  }

}

TEST_CASE("Generate columns for object with eager foreign key relation", "[column][generator][eager]") {
  using namespace matador::test;
  schema s("main");
  s.attach<book>("books");
  s.attach<author>("authors");

  const table books_table{"books"};
  const table authors_table{"authors"};
  const std::vector<column> expected_columns {
    { books_table, "id" },
    { books_table, "title" },
    { authors_table, "id" },
    { authors_table, "first_name" },
    { authors_table, "last_name" },
    { authors_table, "date_of_birth" },
    { authors_table, "year_of_birth" },
    { authors_table, "distinguished" },
    { books_table, "published_in" }
  };
  const auto columns = column_generator::generate<book>(s);

  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(columns[i]));
  }
}
