#include <catch2/catch_test_macros.hpp>

#include <matador/utils/string.hpp>

#include <matador/sql/column_value.hpp>
#include <matador/sql/column_value_visitor.hpp>

#include <variant>
#include <limits>

using namespace matador;

template<typename Type>
void validate_conversion_to_string(Type from, const std::string &expected_string)
{
  sql::column_type value{from};
  sql::column_value_visitor<std::string> visitor;
  std::visit(visitor, const_cast<sql::column_type&>(value));

  REQUIRE(visitor.result == expected_string);
}

TEST_CASE("Test column value visitor", "[column][convert][visitor]") {
  validate_conversion_to_string<char>(-56, "-56");
  validate_conversion_to_string<short>(-511, "-511");
  validate_conversion_to_string<int>(-2345123, "-2345123");
  validate_conversion_to_string<long>(-123456789, "-123456789");
  validate_conversion_to_string<long long>(-987654321, "-987654321");

  validate_conversion_to_string<unsigned char>(56, "56");
  validate_conversion_to_string<unsigned short>(1023, "1023");
  validate_conversion_to_string<unsigned int>(2345123, "2345123");
  validate_conversion_to_string<unsigned long>(123456789, "123456789");
  validate_conversion_to_string<unsigned long long>(987654321, "987654321");

  validate_conversion_to_string<bool>(true, "true");

  validate_conversion_to_string<float>(-0.1234, "-0.1234");
  validate_conversion_to_string<double>(-0.987654321, "-0.987654321");

  validate_conversion_to_string<std::string>("hello", "hello");
  validate_conversion_to_string<const char*>("hello", "hello");

  matador::date today;
  auto expected_string = matador::to_string(today);
  validate_conversion_to_string<matador::date>(today, expected_string);

  matador::time now;
  expected_string = matador::to_string(now);
  validate_conversion_to_string<matador::time>(now, expected_string);
}