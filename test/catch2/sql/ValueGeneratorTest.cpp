#include <catch2/catch_test_macros.hpp>

#include "matador/object/object_ptr.hpp"

#include "matador/sql/value_extractor.hpp"

#include "models/product.hpp"

using namespace matador::sql;

TEST_CASE("Extract values object", "[value extractor]") {
  matador::test::product p;
  p.discontinued = false;
  p.reorder_level = 1;
  p.units_in_order = 2;
  p.units_in_stock = 100;
  p.unit_price = 49;
  p.quantity_per_unit = "pcs";
  p.category = matador::object_ptr<matador::test::category>(new matador::test::category{7, "Stuff"});
  p.supplier = matador::object_ptr<matador::test::supplier>(new matador::test::supplier{13, "ACME"});
  p.product_name = "candle";

  const std::vector<matador::utils::any_type> expected_values {
    std::string{"candle"}, 13UL, 7UL, std::string{"pcs"}, 49U, 100U, 2U, 1U, false
  };
  auto values = value_extractor::extract(p);

  REQUIRE(values == expected_values);
}