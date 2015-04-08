#include "TimeTestUnit.hpp"

#include "tools/time.hpp"
#include "tools/string.hpp"

using namespace oos;

TimeTestUnit::TimeTestUnit()
  : unit_test("time", "time test unit")
{
  add_test("create", std::bind(&TimeTestUnit::test_create, this), "create time");
  add_test("init", std::bind(&TimeTestUnit::test_initialize, this), "init time");
  add_test("invalid", std::bind(&TimeTestUnit::test_invalid, this), "invalid time");
  add_test("copy", std::bind(&TimeTestUnit::test_copy, this), "copy time");
  add_test("assign", std::bind(&TimeTestUnit::test_assign, this), "assign time");
  add_test("compare", std::bind(&TimeTestUnit::test_compare, this), "compare time");
  add_test("modify", std::bind(&TimeTestUnit::test_modify, this), "modify time");
  add_test("parse", std::bind(&TimeTestUnit::test_parse, this), "parse time");
  add_test("format", std::bind(&TimeTestUnit::test_format, this), "format time");
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
  UNIT_ASSERT_EQUAL(tt->tm_hour, now.hour(), "hour of day isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_min, now.minute(), "minute of day isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_sec, now.second(), "second of day isn't equal");

  t = ::time(0);
  tt = localtime(&t);
  oos::time t1 = time::now();

  UNIT_ASSERT_EQUAL(tt->tm_year + 1900, t1.year(), "year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mon + 1, t1.month(), "month of year isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_mday, t1.day(), "day of month isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_hour, t1.hour(), "hour of day isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_min, t1.minute(), "minute of day isn't equal");
  UNIT_ASSERT_EQUAL(tt->tm_sec, t1.second(), "second of day isn't equal");
}

void TimeTestUnit::test_initialize()
{
  oos::time t1(2014, 12, 3, 20, 17, 45);

  UNIT_ASSERT_EQUAL(2014, t1.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t1.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t1.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t1.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t1.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t1.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t1.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_EQUAL(3, t1.day_of_week(), "day of week isn't equal 0");
  UNIT_ASSERT_EQUAL(336, t1.day_of_year(), "day of week isn't equal 0");
  UNIT_ASSERT_FALSE(t1.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t1.is_leapyear(), "time stamp must not be a leap year");

  time_t t;
  ::time(&t);
  struct tm *tt = localtime(&t);

  tt->tm_sec = 45;
  tt->tm_min = 17;
  tt->tm_hour = 20;
  tt->tm_mday = 3;
  tt->tm_mon = 11;
  tt->tm_year = 2014 - 1900;
  tt->tm_isdst = 0;

  oos::time t2(mktime(tt));

  UNIT_ASSERT_EQUAL(2014, t2.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t2.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t2.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t2.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t2.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t2.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t2.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t2.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t2.is_leapyear(), "time stamp must not be a leap year");
}

void TimeTestUnit::test_invalid()
{
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 12, 42, 12, 12, 12), std::logic_error, "date isn't valid", "date should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 13, 31, 12, 12, 12), std::logic_error, "date isn't valid", "date should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 2, 29, 12, 12, 12), std::logic_error, "date isn't valid", "date should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 2, 28, 63, 12, 12), std::logic_error, "time isn't valid", "time should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 2, 28, 12, 63, 12), std::logic_error, "time isn't valid", "time should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 2, 28, 12, 12, 63), std::logic_error, "time isn't valid", "time should not be valid");
  UNIT_ASSERT_EXCEPTION(oos::time(2015, 2, 28, 12, 12, 12, 10000), std::logic_error, "time isn't valid", "time should not be valid");
}

void TimeTestUnit::test_copy()
{
  oos::time t1(2014, 12, 3, 20, 17, 45);
  oos::time t2(t1);

  UNIT_ASSERT_EQUAL(2014, t1.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t1.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t1.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t1.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t1.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t1.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t1.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t1.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t1.is_leapyear(), "time stamp must not be a leap year");

  UNIT_ASSERT_EQUAL(2014, t2.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t2.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t2.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t2.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t2.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t2.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t2.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t2.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t2.is_leapyear(), "time stamp must not be a leap year");

  UNIT_ASSERT_TRUE(t1 == t2, "times must be equal");
}

void TimeTestUnit::test_assign()
{
  oos::time t1(2014, 12, 3, 20, 17, 45);
  oos::time t2;
  t2 = t1;

  UNIT_ASSERT_EQUAL(2014, t1.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t1.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t1.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t1.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t1.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t1.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t1.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t1.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t1.is_leapyear(), "time stamp must not be a leap year");

  UNIT_ASSERT_EQUAL(2014, t2.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(12, t2.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(3, t2.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(20, t2.hour(), "hour of day isn't equal 20");
  UNIT_ASSERT_EQUAL(17, t2.minute(), "minute day isn't equal 17");
  UNIT_ASSERT_EQUAL(45, t2.second(), "second of day isn't equal 45");
  UNIT_ASSERT_EQUAL(0, t2.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t2.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t2.is_leapyear(), "time stamp must not be a leap year");

  UNIT_ASSERT_TRUE(t1 == t2, "times must be equal");
}

void TimeTestUnit::test_compare()
{
  oos::time t1(2015, 1, 31, 11, 35, 7);
  oos::time t2(2014, 12, 3, 20, 17, 45);
  oos::time t3(t1);

  UNIT_ASSERT_TRUE(t1 > t2, "t1 is greater than t2");
  UNIT_ASSERT_TRUE(t2 < t1, "t2 is less than t1");
  UNIT_ASSERT_TRUE(t2 != t1, "t1 is not equal t2");
  UNIT_ASSERT_TRUE(t3 == t1, "t1 is equal t3");
  UNIT_ASSERT_TRUE(t1 >= t2, "t1 is greater equal than t2");
  UNIT_ASSERT_TRUE(t2 <= t1, "t2 is less equal than t1");
  UNIT_ASSERT_TRUE(t1 >= t3, "t1 is greater equal than t2");
  UNIT_ASSERT_TRUE(t3 <= t1, "t2 is less equal than t1");
}

void TimeTestUnit::test_modify()
{
  oos::time t(2015, 1, 31, 11, 35, 7);

  UNIT_ASSERT_EQUAL(2015, t.year(), "year isn't equal 2015");
  UNIT_ASSERT_EQUAL(1, t.month(), "month of year isn't equal 1");
  UNIT_ASSERT_EQUAL(31, t.day(), "day of month isn't equal 31");
  UNIT_ASSERT_EQUAL(11, t.hour(), "hour of day isn't equal 11");
  UNIT_ASSERT_EQUAL(35, t.minute(), "minute day isn't equal 35");
  UNIT_ASSERT_EQUAL(7, t.second(), "second of day isn't equal 7");
  UNIT_ASSERT_EQUAL(0, t.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_FALSE(t.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t.is_leapyear(), "time stamp must not be a leap year");

  t.year(2014).month(8).day(8);

  UNIT_ASSERT_EQUAL(2014, t.year(), "year isn't equal 2014");
  UNIT_ASSERT_EQUAL(8, t.month(), "month of year isn't equal 12");
  UNIT_ASSERT_EQUAL(8, t.day(), "day of month isn't equal 3");
  UNIT_ASSERT_EQUAL(11, t.hour(), "hour of day isn't equal 11");
  UNIT_ASSERT_EQUAL(35, t.minute(), "minute day isn't equal 35");
  UNIT_ASSERT_EQUAL(7, t.second(), "second of day isn't equal 7");
  UNIT_ASSERT_EQUAL(0, t.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_TRUE(t.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_FALSE(t.is_leapyear(), "time stamp must not be a leap year");

  t.year(2016);

  UNIT_ASSERT_EQUAL(2016, t.year(), "year isn't equal 2016");
  UNIT_ASSERT_EQUAL(8, t.month(), "month of year isn't equal 8");
  UNIT_ASSERT_EQUAL(8, t.day(), "day of month isn't equal 8");
  UNIT_ASSERT_EQUAL(11, t.hour(), "hour of day isn't equal 11");
  UNIT_ASSERT_EQUAL(35, t.minute(), "minute day isn't equal 35");
  UNIT_ASSERT_EQUAL(7, t.second(), "second of day isn't equal 7");
  UNIT_ASSERT_EQUAL(0, t.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_TRUE(t.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_TRUE(t.is_leapyear(), "time stamp must be a leap year");

  t.year(2000).hour(23).minute(13).second(1).milli_second(4711);

  UNIT_ASSERT_EQUAL(2000, t.year(), "year isn't equal 2000");
  UNIT_ASSERT_EQUAL(8, t.month(), "month of year isn't equal 8");
  UNIT_ASSERT_EQUAL(8, t.day(), "day of month isn't equal 8");
  UNIT_ASSERT_EQUAL(23, t.hour(), "hour of day isn't equal 11");
  UNIT_ASSERT_EQUAL(13, t.minute(), "minute day isn't equal 35");
  UNIT_ASSERT_EQUAL(1, t.second(), "second of day isn't equal 7");
  UNIT_ASSERT_EQUAL(4711, t.milli_second(), "millisecond of day isn't equal 0");
  UNIT_ASSERT_TRUE(t.is_daylight_saving(), "time stamp must not be daylight saving");
  UNIT_ASSERT_TRUE(t.is_leapyear(), "time stamp must be a leap year");
}

void TimeTestUnit::test_parse()
{
  std::string tstr1("03.04.2015 12:55:12.123");

  oos::time t = oos::time::parse(tstr1, "%d.%m.%Y %H:%M:%S.%f");

  UNIT_ASSERT_EQUAL(2015, t.year(), "year must be 2015");
  UNIT_ASSERT_EQUAL(4, t.month(), "month must be 4");
  UNIT_ASSERT_EQUAL(3, t.day(), "day must be 3");
  UNIT_ASSERT_EQUAL(12, t.hour(), "hour must be 12");
  UNIT_ASSERT_EQUAL(55, t.minute(), "minute must be 55");
  UNIT_ASSERT_EQUAL(12, t.second(), "second must be 12");
  UNIT_ASSERT_EQUAL(123, t.milli_second(), "millisecond must be 123");

  tstr1.assign("12:55:12.123 03.04.2015");

  t = oos::time::parse(tstr1, "%H:%M:%S.%f %d.%m.%Y");

  UNIT_ASSERT_EQUAL(2015, t.year(), "year must be 2015");
  UNIT_ASSERT_EQUAL(4, t.month(), "month must be 4");
  UNIT_ASSERT_EQUAL(3, t.day(), "day must be 3");
  UNIT_ASSERT_EQUAL(12, t.hour(), "hour must be 12");
  UNIT_ASSERT_EQUAL(55, t.minute(), "minute must be 55");
  UNIT_ASSERT_EQUAL(12, t.second(), "second must be 12");
  UNIT_ASSERT_EQUAL(123, t.milli_second(), "millisecond must be 123");

  tstr1.assign("12:55:12 03.04.2015");

  t = oos::time::parse(tstr1, "%H:%M:%S %d.%m.%Y");

  UNIT_ASSERT_EQUAL(2015, t.year(), "year must be 2015");
  UNIT_ASSERT_EQUAL(4, t.month(), "month must be 4");
  UNIT_ASSERT_EQUAL(3, t.day(), "day must be 3");
  UNIT_ASSERT_EQUAL(12, t.hour(), "hour must be 12");
  UNIT_ASSERT_EQUAL(55, t.minute(), "minute must be 55");
  UNIT_ASSERT_EQUAL(12, t.second(), "second must be 12");
  UNIT_ASSERT_EQUAL(0, t.milli_second(), "millisecond must be 0");
}

void TimeTestUnit::test_format()
{
  oos::time t(2015, 1, 31, 11, 35, 7, 123);

  std::string tstr(to_string(t, "%H:%M:%S.%f %d.%m.%Y"));

  UNIT_ASSERT_EQUAL(tstr, "11:35:07.123 31.01.2015", "invalid time string [" + tstr + "]");
}
