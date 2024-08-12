#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column_generator.hpp"
#include "matador/sql/schema.hpp"

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

  auto columns = column_generator::generate<order>(s);

  const std::vector<column> expected_columns = {
    { "order", "order_id", "c01" },
    { "order", "order_date", "c02" },
    { "order", "required_date", "c03" },
    { "order", "shipped_date", "c04" },
    { "order", "ship_via", "c05" },
    { "order", "freight", "c06" },
    { "order", "ship_name", "c07" },
    { "order", "ship_address", "c08" },
    { "order", "ship_city", "c09" },
    { "order", "ship_region", "c10" },
    { "order", "ship_postal_code", "c11" },
    { "order", "ship_country", "c12" },
    { "order_details", "order_details_id", "c13" },
    { "order_details", "order_id", "c14" },
    { "order_details", "product_id", "c15" }
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

  const std::vector<column> expected_columns {
    { "books", "id", "c01" },
    { "books", "title", "c02" },
    { "authors", "id", "c03" },
    { "authors", "first_name", "c04" },
    { "authors", "last_name", "c05" },
    { "authors", "date_of_birth", "c06" },
    { "authors", "year_of_birth", "c07" },
    { "authors", "distinguished", "c08" },
    { "books", "published_in", "c09" }
  };
  auto columns = column_generator::generate<book>(s);

  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(columns[i]));
  }
}
