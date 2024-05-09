#include <catch2/catch_test_macros.hpp>

#include "matador/sql/types.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/types.hpp"

using namespace matador;

TEST_CASE("Test value class", "[value]") {
  value v;

  REQUIRE(v.is_null());
  REQUIRE(v.data_type() == data_type::type_null);
  REQUIRE(v.size() == 0);

  v = 7;

  REQUIRE(v.is_integer());
  REQUIRE(v.data_type() == data_type::type_int);
  REQUIRE(v.size() == 0);
  REQUIRE(v.as<int>() == 7);
  REQUIRE(v.as<long>() == 7);

  v = "test";

  REQUIRE(v.is_varchar());
  REQUIRE(v.data_type() == data_type::type_char_pointer);
  REQUIRE(v.size() == 4);

  v = std::string{"hello"};

  REQUIRE(v.is_varchar());
  REQUIRE(v.data_type() == data_type::type_varchar);
  REQUIRE(v.size() == 5);

  v = 4.5;

  REQUIRE(v.is_floating_point());
  REQUIRE(v.data_type() == data_type::type_double);
  REQUIRE(v.size() == 0);

  v = 6.7f;

  REQUIRE(v.is_floating_point());
  REQUIRE(v.data_type() == data_type::type_float);
  REQUIRE(v.size() == 0);

  v = std::string();

  REQUIRE(v.is_string());
  REQUIRE(v.data_type() == data_type::type_text);
  REQUIRE(v.size() == 0);

  v = true;

  REQUIRE(v.is_bool());
  REQUIRE(v.data_type() == data_type::type_bool);
  REQUIRE(v.size() == 0);

  v = nullptr;

  REQUIRE(v.is_null());
  REQUIRE(v.data_type() == data_type::type_null);
  REQUIRE(v.size() == 0);

  v = matador::utils::blob{ 1, 2, 3, 4 };

  REQUIRE(v.is_blob());
  REQUIRE(v.data_type() == data_type::type_blob);
  REQUIRE(v.size() == 4);
}