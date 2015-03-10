#include "DateTestUnit.hpp"

#include "tools/date.hpp"

using namespace oos;

using std::cout;

DateTestUnit::DateTestUnit()
  : unit_test("date", "date test unit")
{
  add_test("create", std::bind(&DateTestUnit::test_create, this), "create date");
  add_test("julian", std::bind(&DateTestUnit::test_julian_date, this), "julian date");
}

DateTestUnit::~DateTestUnit()
{}

void DateTestUnit::test_create()
{
  time_t t = time(0);
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
  UNIT_ASSERT_EQUAL(15, d.day(), "day of month isn't equal 15");
}

void DateTestUnit::test_initialize() {

}
