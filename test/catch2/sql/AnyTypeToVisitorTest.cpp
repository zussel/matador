#include <catch2/catch_test_macros.hpp>

#include "matador/utils/types.hpp"
#include "matador/utils/any_type_converter.hpp"

using namespace matador::utils;

TEST_CASE("Convert any type to string", "[any type visitor]") {
  any_type_converter<std::string> to_string_visitor;

  any_type value = 6;
  std::visit(to_string_visitor, value);
  REQUIRE(to_string_visitor.result == "6");

  value = 2.5;
  std::visit(to_string_visitor, value);
  REQUIRE(to_string_visitor.result == "2.5");

  value = true;
  std::visit(to_string_visitor, value);
  REQUIRE(to_string_visitor.result == "true");

  value = "hello";
  std::visit(to_string_visitor, value);
  REQUIRE(to_string_visitor.result == "hello");

  value = std::string{"world"};
  std::visit(to_string_visitor, value);
  REQUIRE(to_string_visitor.result == "world");
}

TEST_CASE("Convert any type to integral", "[any type visitor]") {
  any_type_converter<long> to_long_visitor;

  any_type value = 6;
  std::visit(to_long_visitor, value);
  REQUIRE(to_long_visitor.result == 6);

  value = 2.5;
  std::visit(to_long_visitor, value);
  REQUIRE(to_long_visitor.result == 2);

  value = true;
  std::visit(to_long_visitor, value);
  REQUIRE(to_long_visitor.result == 1);

  value = "hello";
  std::visit(to_long_visitor, value);
  REQUIRE(to_long_visitor.result == 0);

  value = std::string{"world"};
  std::visit(to_long_visitor, value);
  REQUIRE(to_long_visitor.result == 0);
}

TEST_CASE("Convert any type to floating point", "[any type visitor]") {
  any_type_converter<double> to_double_visitor;

  any_type value = 6;
  std::visit(to_double_visitor, value);
  REQUIRE(to_double_visitor.result == 6);

  value = 2.5;
  std::visit(to_double_visitor, value);
  REQUIRE(to_double_visitor.result == 2.5);

  value = true;
  std::visit(to_double_visitor, value);
  REQUIRE(to_double_visitor.result == 1);

  value = "hello";
  std::visit(to_double_visitor, value);
  REQUIRE(to_double_visitor.result == 0);

  value = std::string{"world"};
  std::visit(to_double_visitor, value);
  REQUIRE(to_double_visitor.result == 0);
}