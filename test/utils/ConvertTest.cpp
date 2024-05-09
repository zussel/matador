#include "ConvertTest.hpp"

#include "matador/utils/convert.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/types.hpp"

ConvertTest::ConvertTest()
: unit_test("convert", "convert test")
{
  add_test("convert_integral", [this] { test_convert_integral(); }, "convert integral test");
  add_test("convert_floating_point", [this] { test_convert_floating_point(); }, "convert floating point test");
}

using namespace matador::utils;

template<typename From, typename To>
void ConvertTest::validate_conversion(From from)
{
  To to{};
  convert(to, from);

  UNIT_ASSERT_EQUAL(static_cast<To>(from), to);
}

template<typename From, typename To>
void ConvertTest::validate_conversion(From from, To expected_to)
{
  To to{};
  convert(to, from);

  UNIT_ASSERT_EQUAL(to, expected_to);
}

template<typename From>
void ConvertTest::validate_integral_conversion(From from)
{
  validate_conversion<From, char>(from);
  validate_conversion<From, short>(from);
  validate_conversion<From, int>(from);
  validate_conversion<From, long>(from);
  validate_conversion<From, long long>(from);
}

void ConvertTest::test_convert_integral()
{
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

void ConvertTest::test_convert_floating_point()
{
  validate_conversion<float, float>(-0.1);
  validate_conversion<float, double>(-0.1);
  validate_conversion<double, double>(-0.44444);
  validate_conversion<double, float>(-0.44444);
}
