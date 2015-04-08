#include "DateTestUnit.hpp"

#include "tools/date.hpp"
#include <tools/string.hpp>

#include <stdexcept>

using namespace oos;

using std::cout;

DateTestUnit::DateTestUnit()
  : unit_test("date", "date test unit")
{
  add_test("create", std::bind(&DateTestUnit::test_create, this), "create date");
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

DateTestUnit::~DateTestUnit()
{}

void DateTestUnit::test_create()
{
  time_t t = ::time(0);
  struct tm *tt = localtime(&t);

  date now;

  UNIT_ASSERT_EQUAL(tt->tm_year + 1900, now.year(), "year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mon + 1, now.month(), "month of year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mday, now.day(), "day of month isn't equal");
}

void DateTestUnit::test_julian_date()
{
  // 15.3.2015
  date d(2457090);

  UNIT_ASSERT_EQUAL(2015, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_initialize()
{
  date d(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457090, d.julian_date(), "julian date isn't equal 2457090");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_invalid()
{
  UNIT_ASSERT_EXCEPTION(date d(42, 12, 2015), std::logic_error, "date isn't valid", "date should not be valid");
  UNIT_ASSERT_EXCEPTION(date d(31, 13, 2015), std::logic_error, "date isn't valid", "date should not be valid");
  UNIT_ASSERT_EXCEPTION(date d(29, 2, 2015), std::logic_error, "date isn't valid", "date should not be valid");
}

void DateTestUnit::test_copy()
{
  date d1(8, 3, 2015);

  date d2(d1);

  UNIT_ASSERT_TRUE(d1 == d2, "dates must be equal");
  UNIT_ASSERT_EQUAL(2015, d2.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d2.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d2.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457090, d2.julian_date(), "julian date isn't equal 2457090");
  UNIT_ASSERT_FALSE(d2.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d2.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_assign()
{
  date d1(8, 3, 2015);

  date d2;
  d2 = d1;

  UNIT_ASSERT_TRUE(d1 == d2, "dates must be equal");
  UNIT_ASSERT_EQUAL(2015, d2.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d2.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d2.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457090, d2.julian_date(), "julian date isn't equal 2457090");
  UNIT_ASSERT_FALSE(d2.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d2.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_compare()
{
  date d1(8, 3, 2015);
  date d2(27, 8, 2014);
  date d3(d1);

  UNIT_ASSERT_TRUE(d1 > d2, "d1 is greater than d2");
  UNIT_ASSERT_TRUE(d2 < d1, "d2 is less than d1");
  UNIT_ASSERT_TRUE(d2 != d1, "d1 is not equal d2");
  UNIT_ASSERT_TRUE(d3 == d1, "d1 is equal d3");
  UNIT_ASSERT_TRUE(d1 >= d2, "d1 is greater equal than d2");
  UNIT_ASSERT_TRUE(d2 <= d1, "d2 is less equal than d1");
  UNIT_ASSERT_TRUE(d1 >= d3, "d1 is greater equal than d2");
  UNIT_ASSERT_TRUE(d3 <= d1, "d2 is less equal than d1");
}

void DateTestUnit::test_set()
{
  date d;

  d.set(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457090, d.julian_date(), "julian date isn't equal 2457090");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_modify()
{
  date d(8, 3, 2015);

  UNIT_ASSERT_EQUAL(2015, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457090, d.julian_date(), "julian date isn't equal 2457090");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_FALSE(d.is_daylight_saving(), "date is inside daylight saving period");

  d.day(3).month(4);

  UNIT_ASSERT_EQUAL(2015, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(3, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2457116, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  d.year(2014);

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(3, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456751, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  d = d + 5;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(8, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456756, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  d = d - 2;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(6, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456754, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  d += 7;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(13, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456761, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  d -= 13;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(31, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456748, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  date d1 = d++;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(1, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456749, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");
  UNIT_ASSERT_EQUAL(2014, d1.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d1.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(31, d1.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456748, d1.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d1.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d1.is_daylight_saving(), "date is inside daylight saving period");

  d1 = d--;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(31, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456748, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");
  UNIT_ASSERT_EQUAL(2014, d1.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d1.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(1, d1.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456749, d1.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d1.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d1.is_daylight_saving(), "date is inside daylight saving period");

  ++d;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(4, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(1, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456749, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");

  --d;

  UNIT_ASSERT_EQUAL(2014, d.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(3, d.month(), "month of year isn't equal 3");
  UNIT_ASSERT_EQUAL(31, d.day(), "day of month isn't equal 15");
  UNIT_ASSERT_EQUAL(2456748, d.julian_date(), "julian date isn't equal 2457116");
  UNIT_ASSERT_FALSE(d.is_leapyear(), "year isn't a leapp year");
  UNIT_ASSERT_TRUE(d.is_daylight_saving(), "date is inside daylight saving period");
}

void DateTestUnit::test_difference()
{
  date d1(30, 6, 2015);
  date d2(1, 6, 2015);

  int diff = d2.difference(d1);
  UNIT_ASSERT_EQUAL(diff, 29, "difference between d1 and d2 must be 29 days");
}

void DateTestUnit::test_to_string()
{
  date d1(30, 6, 2015);

  std::string str = oos::to_string(d1);

  UNIT_ASSERT_EQUAL(str, "2015-06-30", "result must be '2015-06-30'");
}
