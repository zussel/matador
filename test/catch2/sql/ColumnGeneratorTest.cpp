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

  const auto columns = column_generator::generate<order>(s);

  const std::vector<column> expected_columns = {
    { "order", "order_id", "C01" },
    { "order", "order_date", "C02" },
    { "order", "required_date", "C03" },
    { "order", "shipped_date", "C04" },
    { "order", "ship_via", "C05" },
    { "order", "freight", "C06" },
    { "order", "ship_name", "C07" },
    { "order", "ship_address", "C08" },
    { "order", "ship_city", "C09" },
    { "order", "ship_region", "C10" },
    { "order", "ship_postal_code", "C11" },
    { "order", "ship_country", "C12" },
    { "order_details", "order_details_id", "C13" },
    { "order_details", "order_id", "C14" },
    { "order_details", "product_id", "C15" }
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
    { "books", "id", "C01" },
    { "books", "title", "C02" },
    { "authors", "id", "C03" },
    { "authors", "first_name", "C04" },
    { "authors", "last_name", "C05" },
    { "authors", "date_of_birth", "C06" },
    { "authors", "year_of_birth", "C07" },
    { "authors", "distinguished", "C08" },
    { "books", "published_in", "C09" }
  };
  const auto columns = column_generator::generate<book>(s);

  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(columns[i]));
  }
}
