#include <catch2/catch_test_macros.hpp>

#include "matador/utils/convert.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/types.hpp"
#include "matador/utils/string.hpp"

using namespace matador::utils;

template<typename From, typename To>
void validate_conversion(From from)
{
  To to{};
  convert(to, from);

  REQUIRE(static_cast<To>(from) == to);
}

template<typename From, typename To>
void validate_conversion(From from, To expected_to)
{
  To to{};
  convert(to, from);

  REQUIRE(to == expected_to);
}

template<typename From>
void validate_integral_conversion(From from)
{
  validate_conversion<From, char>(from);
  validate_conversion<From, short>(from);
  validate_conversion<From, int>(from);
  validate_conversion<From, long>(from);
  validate_conversion<From, long long>(from);
}

TEST_CASE("Validate integral conversion", "[convert][integral]") {
  validate_integral_conversion<char>(-56);
  validate_integral_conversion<short>(-127);
  validate_integral_conversion<int>(-1234567);
  validate_integral_conversion<long>(-9876543);
  validate_integral_conversion<long long>(-123456790);

  validate_integral_conversion<unsigned char>(56);
  validate_integral_conversion<unsigned short>(127);
  validate_integral_conversion<unsigned int>(1234567);
  validate_integral_conversion<unsigned long>(9876543);
  validate_integral_conversion<unsigned long long>(123456790);

  validate_conversion<short, char>(513, 1);
  validate_conversion<int, char>(514, 2);
  validate_conversion<long, char>(515, 3);
  validate_conversion<long long, char>(516, 4);
}

TEST_CASE("Validate floating point conversion", "[convert][floating_point]") {
  validate_conversion<float, float>(-0.1f);
  validate_conversion<float, double>(-0.1f);
  validate_conversion<double, double>(-0.44444);
  validate_conversion<double, float>(-0.44444);
}

TEST_CASE("Validate integral to string conversion", "[convert][integral][string]") {
  validate_conversion<char, std::string>(-56, "-56");
  validate_conversion<short, std::string>(-127, "-127");
  validate_conversion<int, std::string>(-1234567, "-1234567");
  validate_conversion<long, std::string>(-9876543, "-9876543");
  validate_conversion<long long, std::string>(-123456790, "-123456790");

  validate_conversion<unsigned char, std::string>(56, "56");
  validate_conversion<unsigned short, std::string>(127, "127");
  validate_conversion<unsigned int, std::string>(1234567, "1234567");
  validate_conversion<unsigned long, std::string>(9876543, "9876543");
  validate_conversion<unsigned long long, std::string>(123456790, "123456790");
}

TEST_CASE("Validate floating point to string conversion", "[convert][floating_point][string]") {
  validate_conversion<float, std::string>(-56.1234f, "-56.1234");
  validate_conversion<double, std::string>(-127.444449, "-127.444449");
}

TEST_CASE("Validate string to integral conversion", "[convert][string][integral]") {
  validate_conversion<std::string, char>("-56", -56);
  validate_conversion<std::string, short>("-127", -127);
  validate_conversion<std::string, int>("-1234567", -1234567);
  validate_conversion<std::string, long>("-9876543", -9876543);
  validate_conversion<std::string, long long>("-123456790", -123456790);

  validate_conversion<std::string, unsigned char>("56", 56);
  validate_conversion<std::string, unsigned short>("127", 127);
  validate_conversion<std::string, unsigned int>("1234567", 1234567);
  validate_conversion<std::string, unsigned long>("9876543", 9876543);
  validate_conversion<std::string, unsigned long long>("123456790", 123456790);

  validate_conversion<const char*, char>("-56", -56);
  validate_conversion<const char*, short>("-127", -127);
  validate_conversion<const char*, int>("-1234567", -1234567);
  validate_conversion<const char*, long>("-9876543", -9876543);
  validate_conversion<const char*, long long>("-123456790", -123456790);

  validate_conversion<const char*, unsigned char>("56", 56);
  validate_conversion<const char*, unsigned short>("127", 127);
  validate_conversion<const char*, unsigned int>("1234567", 1234567);
  validate_conversion<const char*, unsigned long>("9876543", 9876543);
  validate_conversion<const char*, unsigned long long>("123456790", 123456790);
}

TEST_CASE("Validate string to floating point conversion", "[convert][string][floating_point]") {
  validate_conversion<std::string, float>("-56.1234", -56.1234f);
  validate_conversion<std::string, double>("-127.444449", -127.444449);

  validate_conversion<const char*, float>("-56.1234", -56.1234f);
  validate_conversion<const char*, double>("-127.444449", -127.444449);
}

TEST_CASE("Validate blob to blob conversion", "[convert][blob]") {
  blob from{1, 2, 3, 4};
  blob to;
  convert(to, from);

  REQUIRE(from == to);
}

TEST_CASE("Validate blob conversion leads to an exception", "[convert][blob][exception]") {
  blob from{1, 2, 3, 4};
  int to;
  REQUIRE_THROWS_AS(convert(to, from), std::logic_error);
}

TEST_CASE("Validate date to string conversion", "[convert][date][string]") {
  matador::date today;
  const auto expected_string = matador::to_string(today);
  std::string to;

  convert(to, today);

  REQUIRE(expected_string == to);
}

TEST_CASE("Validate date conversion leads to an exception", "[convert][date][exception]") {
  matador::date today;
  int to;
  REQUIRE_THROWS_AS(convert(to, today), std::logic_error);
}

TEST_CASE("Validate time to string conversion", "[convert][time][string]") {
  matador::time now;
  const auto expected_string = matador::to_string(now);
  std::string to;

  convert(to, now);

  REQUIRE(expected_string == to);
}

TEST_CASE("Validate time conversion leads to an exception", "[convert][time][exception]") {
  matador::time now;
  int to;
  REQUIRE_THROWS_AS(convert(to, now), std::logic_error);
}
