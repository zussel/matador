#include <catch2/catch_test_macros.hpp>

#include "matador/sql/field.hpp"

using namespace matador;

TEST_CASE("Field test", "[field]") {
  sql::field f("name");

  REQUIRE(f.name() == "name");
  REQUIRE(f.index() == -1);
  REQUIRE(!f.is_unknown());
  REQUIRE(f.is_null());
  REQUIRE(!f.is_integer());
  REQUIRE(!f.is_floating_point());
  REQUIRE(!f.is_blob());
  REQUIRE(!f.is_bool());
  REQUIRE(!f.is_string());

  f = 7UL;
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

  f = sql::field("name", utils::blob{ 7,8,6,5,4,3 }, 0, 1);
  REQUIRE(f.index() == 1);
  REQUIRE(!f.is_null());
  REQUIRE(!f.is_integer());
  REQUIRE(!f.is_floating_point());
  REQUIRE(f.is_blob());
  REQUIRE(!f.is_bool());
  REQUIRE(!f.is_string());

  auto blob_val = f.as<utils::blob>();
  REQUIRE(blob_val.has_value());
  REQUIRE(blob_val.value() == utils::blob{ 7,8,6,5,4,3 });

  REQUIRE_THROWS_AS(f.as<std::string>(), std::logic_error);
}