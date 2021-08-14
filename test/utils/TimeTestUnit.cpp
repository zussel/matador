#include "TimeTestUnit.hpp"

#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include <stdexcept>

using namespace matador;

TimeTestUnit::TimeTestUnit()
  : unit_test("time", "time test unit")
{
  add_test("create", [this] { test_create(); }, "create time");
  add_test("init", [this] { test_initialize(); }, "init time");
  add_test("invalid", [this] { test_invalid(); }, "invalid time");
  add_test("copy", [this] { test_copy(); }, "copy time");
  add_test("assign", [this] { test_assign(); }, "assign time");
  add_test("compare", [this] { test_compare(); }, "compare time");
  add_test("modify", [this] { test_modify(); }, "modify time");
  add_test("parse", [this] { test_parse(); }, "parse time");
  add_test("format", [this] { test_format(); }, "format time");
  add_test("to_date", [this] { test_to_date(); }, "time to date");
  add_test("strftime", [this] { test_strftime(); }, "test strftime");
}

void TimeTestUnit::test_create()
{
  time_t t = ::time(nullptr);
#ifdef _MSC_VER
  struct tm *tt = new tm;
  localtime_s(tt, &t);
#else
  struct tm *tt = localtime(&t);
#endif

  matador::time now(t);

  UNIT_ASSERT_EQUAL(tt->tm_year + 1900, now.year());
  UNIT_ASSERT_EQUAL(tt->tm_mon + 1, now.month());
  UNIT_ASSERT_EQUAL(tt->tm_mday, now.day());
  UNIT_ASSERT_EQUAL(tt->tm_hour, now.hour());
  UNIT_ASSERT_EQUAL(tt->tm_min, now.minute());
  UNIT_ASSERT_EQUAL(tt->tm_sec, now.second());

  matador::time t1 = time::now();

  struct tm ttt = t1.get_tm();
  
  UNIT_ASSERT_EQUAL(ttt.tm_year + 1900, t1.year());
  UNIT_ASSERT_EQUAL(ttt.tm_mon + 1, t1.month());
  UNIT_ASSERT_EQUAL(ttt.tm_mday, t1.day());
  UNIT_ASSERT_EQUAL(ttt.tm_hour, t1.hour());
  UNIT_ASSERT_EQUAL(ttt.tm_min, t1.minute());
  UNIT_ASSERT_EQUAL(ttt.tm_sec, t1.second());
}

void TimeTestUnit::test_initialize()
{
  matador::time t1(2014, 12, 3, 20, 17, 45);

  UNIT_ASSERT_EQUAL(2014, t1.year());
  UNIT_ASSERT_EQUAL(12, t1.month());
  UNIT_ASSERT_EQUAL(3, t1.day());
  UNIT_ASSERT_EQUAL(20, t1.hour());
  UNIT_ASSERT_EQUAL(17, t1.minute());
  UNIT_ASSERT_EQUAL(45, t1.second());
  UNIT_ASSERT_EQUAL(0, t1.milli_second());
  UNIT_ASSERT_EQUAL(3, t1.day_of_week());
  UNIT_ASSERT_EQUAL(336, t1.day_of_year());
  UNIT_ASSERT_FALSE(t1.is_daylight_saving());
  UNIT_ASSERT_FALSE(t1.is_leapyear());

  time_t t;
  ::time(&t);
#ifdef _MSC_VER
  struct tm *tt = new tm;
  localtime_s(tt, &t);
#else
  struct tm *tt = localtime(&t);
#endif

  tt->tm_sec = 45;
  tt->tm_min = 17;
  tt->tm_hour = 20;
  tt->tm_mday = 3;
  tt->tm_mon = 11;
  tt->tm_year = 2014 - 1900;
  tt->tm_isdst = 0;

  matador::time t2(mktime(tt));

  UNIT_ASSERT_EQUAL(2014, t2.year());
  UNIT_ASSERT_EQUAL(12, t2.month());
  UNIT_ASSERT_EQUAL(3, t2.day());
  UNIT_ASSERT_EQUAL(20, t2.hour());
  UNIT_ASSERT_EQUAL(17, t2.minute());
  UNIT_ASSERT_EQUAL(45, t2.second());
  UNIT_ASSERT_EQUAL(0, t2.milli_second());
  UNIT_ASSERT_FALSE(t2.is_daylight_saving());
  UNIT_ASSERT_FALSE(t2.is_leapyear());
}

void TimeTestUnit::test_invalid()
{
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 12, 42, 12, 12, 12), std::logic_error, "date isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 13, 31, 12, 12, 12), std::logic_error, "date isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 2, 29, 12, 12, 12), std::logic_error, "date isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 2, 28, 63, 12, 12), std::logic_error, "time isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 2, 28, 12, 63, 12), std::logic_error, "time isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 2, 28, 12, 12, 63), std::logic_error, "time isn't valid");
  UNIT_ASSERT_EXCEPTION(matador::time(2015, 2, 28, 12, 12, 12, 10000), std::logic_error, "time isn't valid");
}

void TimeTestUnit::test_copy()
{
  matador::time t1(2014, 12, 3, 20, 17, 45);
  matador::time t2(t1);

  UNIT_ASSERT_EQUAL(2014, t1.year());
  UNIT_ASSERT_EQUAL(12, t1.month());
  UNIT_ASSERT_EQUAL(3, t1.day());
  UNIT_ASSERT_EQUAL(20, t1.hour());
  UNIT_ASSERT_EQUAL(17, t1.minute());
  UNIT_ASSERT_EQUAL(45, t1.second());
  UNIT_ASSERT_EQUAL(0, t1.milli_second());
  UNIT_ASSERT_FALSE(t1.is_daylight_saving());
  UNIT_ASSERT_FALSE(t1.is_leapyear());

  UNIT_ASSERT_EQUAL(2014, t2.year());
  UNIT_ASSERT_EQUAL(12, t2.month());
  UNIT_ASSERT_EQUAL(3, t2.day());
  UNIT_ASSERT_EQUAL(20, t2.hour());
  UNIT_ASSERT_EQUAL(17, t2.minute());
  UNIT_ASSERT_EQUAL(45, t2.second());
  UNIT_ASSERT_EQUAL(0, t2.milli_second());
  UNIT_ASSERT_FALSE(t2.is_daylight_saving());
  UNIT_ASSERT_FALSE(t2.is_leapyear());

  UNIT_ASSERT_TRUE(t1 == t2);
}

void TimeTestUnit::test_assign()
{
  matador::time t1(2014, 12, 3, 20, 17, 45);
  matador::time t2;
  t2 = t1;

  UNIT_ASSERT_EQUAL(2014, t1.year());
  UNIT_ASSERT_EQUAL(12, t1.month());
  UNIT_ASSERT_EQUAL(3, t1.day());
  UNIT_ASSERT_EQUAL(20, t1.hour());
  UNIT_ASSERT_EQUAL(17, t1.minute());
  UNIT_ASSERT_EQUAL(45, t1.second());
  UNIT_ASSERT_EQUAL(0, t1.milli_second());
  UNIT_ASSERT_FALSE(t1.is_daylight_saving());
  UNIT_ASSERT_FALSE(t1.is_leapyear());

  UNIT_ASSERT_EQUAL(2014, t2.year());
  UNIT_ASSERT_EQUAL(12, t2.month());
  UNIT_ASSERT_EQUAL(3, t2.day());
  UNIT_ASSERT_EQUAL(20, t2.hour());
  UNIT_ASSERT_EQUAL(17, t2.minute());
  UNIT_ASSERT_EQUAL(45, t2.second());
  UNIT_ASSERT_EQUAL(0, t2.milli_second());
  UNIT_ASSERT_FALSE(t2.is_daylight_saving());
  UNIT_ASSERT_FALSE(t2.is_leapyear());

  UNIT_ASSERT_TRUE(t1 == t2);
}

void TimeTestUnit::test_compare()
{
  matador::time t1(2015, 1, 31, 11, 35, 7);
  matador::time t2(2014, 12, 3, 20, 17, 45);
  matador::time t3(t1);

  UNIT_ASSERT_TRUE(t1 > t2);
  UNIT_ASSERT_TRUE(t2 < t1);
  UNIT_ASSERT_TRUE(t2 != t1);
  UNIT_ASSERT_TRUE(t3 == t1);
  UNIT_ASSERT_TRUE(t1 >= t2);
  UNIT_ASSERT_TRUE(t2 <= t1);
  UNIT_ASSERT_TRUE(t1 >= t3);
  UNIT_ASSERT_TRUE(t3 <= t1);
}

void TimeTestUnit::test_modify()
{
  matador::time t(2015, 1, 31, 11, 35, 7);

  UNIT_ASSERT_EQUAL(2015, t.year());
  UNIT_ASSERT_EQUAL(1, t.month());
  UNIT_ASSERT_EQUAL(31, t.day());
  UNIT_ASSERT_EQUAL(11, t.hour());
  UNIT_ASSERT_EQUAL(35, t.minute());
  UNIT_ASSERT_EQUAL(7, t.second());
  UNIT_ASSERT_EQUAL(0, t.milli_second());
  UNIT_ASSERT_FALSE(t.is_daylight_saving());
  UNIT_ASSERT_FALSE(t.is_leapyear());

  t.year(2014).month(8).day(8);

  UNIT_ASSERT_EQUAL(2014, t.year());
  UNIT_ASSERT_EQUAL(8, t.month());
  UNIT_ASSERT_EQUAL(8, t.day());
  UNIT_ASSERT_EQUAL(11, t.hour());
  UNIT_ASSERT_EQUAL(35, t.minute());
  UNIT_ASSERT_EQUAL(7, t.second());
  UNIT_ASSERT_EQUAL(0, t.milli_second());
//  UNIT_ASSERT_TRUE(t.is_daylight_saving());
  UNIT_ASSERT_FALSE(t.is_leapyear());

  t.year(2016);

  UNIT_ASSERT_EQUAL(2016, t.year());
  UNIT_ASSERT_EQUAL(8, t.month());
  UNIT_ASSERT_EQUAL(8, t.day());
  UNIT_ASSERT_EQUAL(11, t.hour());
  UNIT_ASSERT_EQUAL(35, t.minute());
  UNIT_ASSERT_EQUAL(7, t.second());
  UNIT_ASSERT_EQUAL(0, t.milli_second());
//  UNIT_ASSERT_TRUE(t.is_daylight_saving());
  UNIT_ASSERT_TRUE(t.is_leapyear());

  t.year(2000).hour(23).minute(13).second(1).milli_second(4711);

  UNIT_ASSERT_EQUAL(2000, t.year());
  UNIT_ASSERT_EQUAL(8, t.month());
  UNIT_ASSERT_EQUAL(8, t.day());
  UNIT_ASSERT_EQUAL(23, t.hour());
  UNIT_ASSERT_EQUAL(13, t.minute());
  UNIT_ASSERT_EQUAL(1, t.second());
  UNIT_ASSERT_EQUAL(4711, t.milli_second());
//  UNIT_ASSERT_TRUE(t.is_daylight_saving());
  UNIT_ASSERT_TRUE(t.is_leapyear());
}

void TimeTestUnit::test_parse()
{
  std::string tstr1("03.04.2015 12:55:12.123");

  matador::time t = matador::time::parse(tstr1, "%d.%m.%Y %H:%M:%S.%f");

  UNIT_ASSERT_EQUAL(2015, t.year());
  UNIT_ASSERT_EQUAL(4, t.month());
  UNIT_ASSERT_EQUAL(3, t.day());
  UNIT_ASSERT_EQUAL(12, t.hour());
  UNIT_ASSERT_EQUAL(55, t.minute());
  UNIT_ASSERT_EQUAL(12, t.second());
  UNIT_ASSERT_EQUAL(123, t.milli_second());

  tstr1.assign("12:55:12.123 03.04.2015");

  t = matador::time::parse(tstr1, "%H:%M:%S.%f %d.%m.%Y");

  UNIT_ASSERT_EQUAL(2015, t.year());
  UNIT_ASSERT_EQUAL(4, t.month());
  UNIT_ASSERT_EQUAL(3, t.day());
  UNIT_ASSERT_EQUAL(12, t.hour());
  UNIT_ASSERT_EQUAL(55, t.minute());
  UNIT_ASSERT_EQUAL(12, t.second());
  UNIT_ASSERT_EQUAL(123, t.milli_second());

  tstr1.assign("12:55:12 03.04.2015");

  t = matador::time::parse(tstr1, "%H:%M:%S %d.%m.%Y");

  UNIT_ASSERT_EQUAL(2015, t.year());
  UNIT_ASSERT_EQUAL(4, t.month());
  UNIT_ASSERT_EQUAL(3, t.day());
  UNIT_ASSERT_EQUAL(12, t.hour());
  UNIT_ASSERT_EQUAL(55, t.minute());
  UNIT_ASSERT_EQUAL(12, t.second());
  UNIT_ASSERT_EQUAL(0, t.milli_second());
}

void TimeTestUnit::test_format()
{
  matador::time t(2015, 1, 31, 11, 35, 7, 123);

  std::string tstr(to_string(t, "%H:%M:%S.%f %d.%m.%Y"));

  UNIT_ASSERT_EQUAL(tstr, "11:35:07.123 31.01.2015");
}

void TimeTestUnit::test_to_date()
{
  matador::time t1(2014, 12, 3, 20, 17, 45);

  UNIT_ASSERT_EQUAL(2014, t1.year());
  UNIT_ASSERT_EQUAL(12, t1.month());
  UNIT_ASSERT_EQUAL(3, t1.day());
  UNIT_ASSERT_EQUAL(20, t1.hour());
  UNIT_ASSERT_EQUAL(17, t1.minute());
  UNIT_ASSERT_EQUAL(45, t1.second());
  UNIT_ASSERT_EQUAL(0, t1.milli_second());
  UNIT_ASSERT_EQUAL(3, t1.day_of_week());
  UNIT_ASSERT_EQUAL(336, t1.day_of_year());
  UNIT_ASSERT_FALSE(t1.is_daylight_saving());
  UNIT_ASSERT_FALSE(t1.is_leapyear());

  matador::date d1 = t1.to_date();

  UNIT_ASSERT_EQUAL(2014, d1.year());
  UNIT_ASSERT_EQUAL(12, d1.month());
  UNIT_ASSERT_EQUAL(3, d1.day());
}

void TimeTestUnit::test_strftime()
{
  matador::time t(2019, 8, 29, 14, 43, 57, 123);

  struct timeval tv = t.get_timeval();

  char buffer[64];

  strftime(buffer, 64, "%H:%M:%S.%f", &tv);

  UNIT_EXPECT_EQUAL("14:43:57.123", buffer);

  strftime(buffer, 64, "%H:%M:%S.%f %Y", &tv);

  UNIT_EXPECT_EQUAL("14:43:57.123 2019", buffer);
}
