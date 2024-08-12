#include <catch2/catch_test_macros.hpp>

#include "matador/sql/value.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/types.hpp"

using namespace matador;

TEST_CASE("Test value class", "[value]") {
  sql::value v;

  REQUIRE(v.is_null());
  REQUIRE(v.type() == data_type::type_null);
  REQUIRE(v.size() == 0);

  v = 7;

  REQUIRE(v.is_integer());
  REQUIRE(v.type() == data_type::type_int);
  REQUIRE(v.size() == 0);
  REQUIRE(v.as<int>() == 7);
  REQUIRE(v.as<long>() == 7);

  v = "test";

  REQUIRE(v.is_varchar());
  REQUIRE(v.type() == data_type::type_char_pointer);
  REQUIRE(v.size() == 4);

  v = std::string{"hello"};

  REQUIRE(v.is_varchar());
  REQUIRE(v.type() == data_type::type_varchar);
  REQUIRE(v.size() == 5);

  v = 4.5;

  REQUIRE(v.is_floating_point());
  REQUIRE(v.type() == data_type::type_double);
  REQUIRE(v.size() == 0);

  v = 6.7f;

  REQUIRE(v.is_floating_point());
  REQUIRE(v.type() == data_type::type_float);
  REQUIRE(v.size() == 0);

  v = std::string();

  REQUIRE(v.is_string());
  REQUIRE(v.type() == data_type::type_text);
  REQUIRE(v.size() == 0);

  v = true;

  REQUIRE(v.is_bool());
  REQUIRE(v.type() == data_type::type_bool);
  REQUIRE(v.size() == 0);

  v = nullptr;

  REQUIRE(v.is_null());
  REQUIRE(v.type() == data_type::type_null);
  REQUIRE(v.size() == 0);

  v = matador::utils::blob{ 1, 2, 3, 4 };

  REQUIRE(v.is_blob());
  REQUIRE(v.type() == data_type::type_blob);
  REQUIRE(v.size() == 4);

  const auto today = date{};
  v = today;

  REQUIRE(v.is_date());
  REQUIRE(v.type() == data_type::type_date);
  REQUIRE(v.size() == 0);
  REQUIRE(v.as<date>() == today);

  const auto now = matador::time{};
  v = now;

  REQUIRE(v.is_time());
  REQUIRE(v.type() == data_type::type_time);
  REQUIRE(v.size() == 0);
  REQUIRE(v.as<matador::time>() == now);
}