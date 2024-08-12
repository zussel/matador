#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column_definition_generator.hpp"
#include "matador/sql/schema.hpp"

#include "matador/utils/data_types.hpp"

#include "models/product.hpp"
#include "models/optional.hpp"

using namespace matador::sql;
using namespace matador::utils;

TEST_CASE("Generate column definitions from object", "[column][definition][generator]") {
  schema repo("main");

  auto columns = column_definition_generator::generate<matador::test::product>(repo);

  const std::vector<column_definition> expected_columns = {
    column_definition{"product_name", matador::data_type::type_varchar, constraints::PRIMARY_KEY, null_option::NOT_NULL },
    column_definition{"supplier_id", matador::data_type::type_unsigned_long, constraints::FOREIGN_KEY, null_option::NOT_NULL },
    column_definition{"category_id", matador::data_type::type_unsigned_long, constraints::FOREIGN_KEY, null_option::NOT_NULL },
    column_definition{"quantity_per_unit", matador::data_type::type_varchar, null_attributes, null_option::NOT_NULL },
    column_definition{"unit_price", matador::data_type::type_unsigned_int, null_attributes, null_option::NOT_NULL },
    column_definition{"units_in_stock", matador::data_type::type_unsigned_int, null_attributes, null_option::NOT_NULL },
    column_definition{"units_in_order", matador::data_type::type_unsigned_int, null_attributes, null_option::NOT_NULL },
    column_definition{"reorder_level", matador::data_type::type_unsigned_int, null_attributes, null_option::NOT_NULL },
    column_definition{"discontinued", matador::data_type::type_bool, null_attributes, null_option::NOT_NULL }
  };
  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());

  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].name() == columns[i].name());
    REQUIRE(expected_columns[i].attributes().options() == columns[i].attributes().options() );
    REQUIRE(expected_columns[i].type() == columns[i].type() );
  }
}

TEST_CASE("Generate columns from object with nullable columns", "[column generator]") {
  schema repo("main");

  auto columns = column_definition_generator::generate<matador::test::optional>(repo);

  const std::vector<column_definition> expected_columns = {
    column_definition{"id", matador::data_type::type_unsigned_long, constraints::PRIMARY_KEY, null_option::NOT_NULL },
    column_definition{"name", matador::data_type::type_varchar, null_attributes, null_option::NOT_NULL },
    column_definition{"age", matador::data_type::type_unsigned_int, null_attributes, null_option::NOT_NULL }
  };
  REQUIRE(!columns.empty());
  REQUIRE(columns.size() == expected_columns.size());

  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].name() == columns[i].name());
    REQUIRE(expected_columns[i].attributes().options() == columns[i].attributes().options() );
    REQUIRE(expected_columns[i].type() == columns[i].type() );
  }
}