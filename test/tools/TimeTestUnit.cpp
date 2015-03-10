#include "TimeTestUnit.hpp"

#include "tools/time.hpp"

using namespace oos;

TimeTestUnit::TimeTestUnit()
  : unit_test("time", "time test unit")
{
  add_test("create", std::bind(&TimeTestUnit::test_create, this), "create date");
  add_test("init", std::bind(&TimeTestUnit::test_initialize, this), "init date");
}

TimeTestUnit::~TimeTestUnit()
{}

void TimeTestUnit::test_create()
{
  time_t t = ::time(0);
  struct tm *tt = localtime(&t);

  oos::time now;

  UNIT_ASSERT_EQUAL(tt->tm_year + 1900, now.year(), "year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mon + 1, now.month(), "month of year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mday, now.day(), "day of month isn't equal");
}

void TimeTestUnit::test_initialize()
{
  oos::time t(2014, 12, 3, 20, 17, 45);

  UNIT_ASSERT_EQUAL(2014, t.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t.is_leapyear(), "time stamp must not be a leap year");
}
