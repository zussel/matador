#include "DateTestUnit.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/string.hpp"

#include <ctime>
#include <stdexcept>

using namespace matador;

DateTestUnit::DateTestUnit()
  : unit_test("date", "date test unit")
{
  add_test("create", std::bind(&DateTestUnit::test_create, this), "create date");
  add_test("parse", std::bind(&DateTestUnit::test_parse, this), "parse date");
  add_test("julian", std::bind(&DateTestUnit::test_julian_date, this), "julian date");
  add_test("init", std::bind(&DateTestUnit::test_initialize, this), "initialize date");
  add_test("invalid", std::bind(&DateTestUnit::test_invalid, this), "invalid date");
  add_test("copy", std::bind(&DateTestUnit::test_copy, this), "copy date");
  add_test("assign", std::bind(&DateTestUnit::test_assign, this), "assign date");
  add_test("compare", std::bind(&DateTestUnit::test_compare, this), "compare date");
  add_test("set", std::bind(&DateTestUnit::test_set, this), "set date");
  add_test("modify", std::bind(&DateTestUnit::test_modify, this), "modify date");
  add_test("difference", std::bind(&DateTestUnit::test_difference, this), "difference date");
  add_test("to_string", std::bind(&DateTestUnit::test_to_string, this), "to string");
}

void DateTestUnit::test_create()
{
  time_t t = std::time(nullptr);
#ifdef _MSC_VER
  struct tm *tt = new tm;
  localtime_s(tt, &t);
#else
  struct tm *tt = localtime(&t);
#endif

  date now;

  UNIT_ASSERT_EQUAL(tt->tm_year + 1900, now.year());
  UNIT_ASSERT_EQUAL(tt->tm_mon + 1, now.month());
  UNIT_ASSERT_EQUAL(tt->tm_mday, now.day());
}

void DateTestUnit::test_parse()
{
  date d = date::parse("13-11-1979", "%d-%m-%Y");

  UNIT_ASSERT_EQUAL(d.day(), 13);
  UNIT_ASSERT_EQUAL(d.month(), 11);
  UNIT_ASSERT_EQUAL(d.year(), 1979);

  d = date::parse("2000-11-24", date_format::ISO8601);

  UNIT_ASSERT_EQUAL(d.day(), 24);
  UNIT_ASSERT_EQUAL(d.month(), 11);
  UNIT_ASSERT_EQUAL(d.year(), 2000);

  d.set("12/23/2012", "%m/%d/%Y");

  UNIT_ASSERT_EQUAL(d.day(), 23);
  UNIT_ASSERT_EQUAL(d.month(), 12);
  UNIT_ASSERT_EQUAL(d.year(), 2012);
}

void DateTestUnit::test_julian_date()
{
  // 15.3.2015
  date d(2457090);

  UNIT_ASSERT_EQUAL(2015, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(8, d.day());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_FALSE(d.is_daylight_saving());
}

void DateTestUnit::test_initialize()
{
  date d(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(8, d.day());
  UNIT_ASSERT_EQUAL(2457090, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_FALSE(d.is_daylight_saving());
}

void DateTestUnit::test_invalid()
{
  UNIT_ASSERT_EXCEPTION(date d(42, 12, 2015), std::logic_error, "date isn't valid");
  UNIT_ASSERT_EXCEPTION(date d(31, 13, 2015), std::logic_error, "date isn't valid");
  UNIT_ASSERT_EXCEPTION(date d(29, 2, 2015), std::logic_error, "date isn't valid");
}

void DateTestUnit::test_copy()
{
  date d1(8, 3, 2015);

  date d2(d1);

  UNIT_ASSERT_TRUE(d1 == d2);
  UNIT_ASSERT_EQUAL(2015, d2.year());
  UNIT_ASSERT_EQUAL(3, d2.month());
  UNIT_ASSERT_EQUAL(8, d2.day());
  UNIT_ASSERT_EQUAL(2457090, d2.julian_date());
  UNIT_ASSERT_FALSE(d2.is_leapyear());
  UNIT_ASSERT_FALSE(d2.is_daylight_saving());
}

void DateTestUnit::test_assign()
{
  date d1(8, 3, 2015);

  date d2;
  d2 = d1;

  UNIT_ASSERT_TRUE(d1 == d2);
  UNIT_ASSERT_EQUAL(2015, d2.year());
  UNIT_ASSERT_EQUAL(3, d2.month());
  UNIT_ASSERT_EQUAL(8, d2.day());
  UNIT_ASSERT_EQUAL(2457090, d2.julian_date());
  UNIT_ASSERT_FALSE(d2.is_leapyear());
  UNIT_ASSERT_FALSE(d2.is_daylight_saving());
}

void DateTestUnit::test_compare()
{
  date d1(8, 3, 2015);
  date d2(27, 8, 2014);
  date d3(d1);

  UNIT_ASSERT_TRUE(d1 > d2);
  UNIT_ASSERT_TRUE(d2 < d1);
  UNIT_ASSERT_TRUE(d2 != d1);
  UNIT_ASSERT_TRUE(d3 == d1);
  UNIT_ASSERT_TRUE(d1 >= d2);
  UNIT_ASSERT_TRUE(d2 <= d1);
  UNIT_ASSERT_TRUE(d1 >= d3);
  UNIT_ASSERT_TRUE(d3 <= d1);
}

void DateTestUnit::test_set()
{
  date d;

  d.set(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(8, d.day());
  UNIT_ASSERT_EQUAL(2457090, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_FALSE(d.is_daylight_saving());
}

void DateTestUnit::test_modify()
{
  date d(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(8, d.day());
  UNIT_ASSERT_EQUAL(2457090, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_FALSE(d.is_daylight_saving());

  d.day(3).month(4);

  UNIT_ASSERT_EQUAL(2015, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(3, d.day());
  UNIT_ASSERT_EQUAL(2457116, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  d.year(2014);

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(3, d.day());
  UNIT_ASSERT_EQUAL(2456751, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  d = d + 5;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(8, d.day());
  UNIT_ASSERT_EQUAL(2456756, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  d = d - 2;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(6, d.day());
  UNIT_ASSERT_EQUAL(2456754, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  d += 7;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(13, d.day());
  UNIT_ASSERT_EQUAL(2456761, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  d -= 13;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(31, d.day());
  UNIT_ASSERT_EQUAL(2456748, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  date d1 = d++;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(1, d.day());
  UNIT_ASSERT_EQUAL(2456749, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());
  UNIT_ASSERT_EQUAL(2014, d1.year());
  UNIT_ASSERT_EQUAL(3, d1.month());
  UNIT_ASSERT_EQUAL(31, d1.day());
  UNIT_ASSERT_EQUAL(2456748, d1.julian_date());
  UNIT_ASSERT_FALSE(d1.is_leapyear());
  UNIT_ASSERT_TRUE(d1.is_daylight_saving());

  d1 = d--;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(31, d.day());
  UNIT_ASSERT_EQUAL(2456748, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());
  UNIT_ASSERT_EQUAL(2014, d1.year());
  UNIT_ASSERT_EQUAL(4, d1.month());
  UNIT_ASSERT_EQUAL(1, d1.day());
  UNIT_ASSERT_EQUAL(2456749, d1.julian_date());
  UNIT_ASSERT_FALSE(d1.is_leapyear());
  UNIT_ASSERT_TRUE(d1.is_daylight_saving());

  ++d;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(4, d.month());
  UNIT_ASSERT_EQUAL(1, d.day());
  UNIT_ASSERT_EQUAL(2456749, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());

  --d;

  UNIT_ASSERT_EQUAL(2014, d.year());
  UNIT_ASSERT_EQUAL(3, d.month());
  UNIT_ASSERT_EQUAL(31, d.day());
  UNIT_ASSERT_EQUAL(2456748, d.julian_date());
  UNIT_ASSERT_FALSE(d.is_leapyear());
  UNIT_ASSERT_TRUE(d.is_daylight_saving());
}

void DateTestUnit::test_difference()
{
  date d1(30, 6, 2015);
  date d2(1, 6, 2015);

  int diff = d2.difference(d1);
  UNIT_ASSERT_EQUAL(diff, 29);
}

void DateTestUnit::test_to_string()
{
  date d1(30, 6, 2015);

  std::string str = matador::to_string(d1);

  UNIT_ASSERT_EQUAL(str, "2015-06-30");
}
