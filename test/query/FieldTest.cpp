#include <catch2/catch_test_macros.hpp>

#include "matador/query/field.hpp"

using namespace matador;

TEST_CASE("Test field", "[field]") {
  field f("name");

  REQUIRE(f.name() == "name");
  REQUIRE(f.index() == -1);
  REQUIRE(f.is_null());
  REQUIRE(!f.is_integer());
  REQUIRE(!f.is_floating_point());
  REQUIRE(!f.is_blob());
  REQUIRE(!f.is_bool());
  REQUIRE(!f.is_string());

  f = 7U;
  REQUIRE(!f.is_null());
  REQUIRE(f.is_integer());
  REQUIRE(!f.is_floating_point());
  REQUIRE(!f.is_blob());
  REQUIRE(!f.is_bool());
  REQUIRE(!f.is_string());

  auto int_val = f.as<int>();
  REQUIRE(int_val.has_value());
  REQUIRE(int_val.value() == 7);
  auto float_val = f.as<float>();
  REQUIRE(float_val.has_value());
  REQUIRE(float_val.value() == 7.0);
  auto str_val = f.as<std::string>();
  REQUIRE(str_val.has_value());
  REQUIRE(str_val.value() == "7");
  auto bool_val = f.as<bool>();
  REQUIRE(bool_val.has_value());
  REQUIRE(bool_val.value());

  f = field("name", utils::blob_type_t{ 7,8,6,5,4,3 }, column_constraint::None, 0, 1);
  REQUIRE(f.index() == 1);
  REQUIRE(!f.is_null());
  REQUIRE(!f.is_integer());
  REQUIRE(!f.is_floating_point());
  REQUIRE(f.is_blob());
  REQUIRE(!f.is_bool());
  REQUIRE(!f.is_string());

  auto blob_val = f.as<utils::blob_type_t>();
  REQUIRE(blob_val.has_value());
  REQUIRE(blob_val.value() == utils::blob_type_t{ 7,8,6,5,4,3 });

  REQUIRE(!f.as<std::string>().has_value());
}