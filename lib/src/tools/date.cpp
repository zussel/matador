#include "tools/date.hpp"
#include "tools/calendar.h"

#include <ctime>

namespace oos {

const unsigned char date::month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

date::details_t::details_t(){
  // should calc now
  time_t t = time(0);
  struct tm *now = localtime(&t);
  day = now->tm_mday;
  month = now->tm_mon + 1;
  year = now->tm_year + 1900;
  is_daylight_saving = now->tm_isdst > 0;
  is_leap_year = date::is_leapyear(year);
  julian_date = gc2jd(day, month, year);
}

date::details_t::details_t(int jd)
  : julian_date(jd)
{}

date::details_t::details_t(int y, int m, int d)
  : day(d), month(m), year(y)
{}

date::details_t::details_t(const details_t &x)
  : day(x.day), month(x.month), year(x.year)
  , is_daylight_saving(x.is_daylight_saving)
  , is_leap_year(x.is_leap_year)
{}

date::date()
{}

date::date(int julian_date) : details_(julian_date) {
  jd2gc(details_.julian_date, details_.day, details_.month, details_.year);
}

date::date(int day, int month, int year)
  : details_(year, month, day)
{
  // calculate julian date
  details_.julian_date = gc2jd(details_.day, details_.month, details_.year);
}

date::date(const char *stamp, const char *format) {
  this->set(stamp, format);
}

date::date(const date &x) :
  details_(x.details_)
{}

date &
date::operator=(const date &x) {
  details_.day = x.details_.day;
  details_.month = x.details_.month;
  details_.year = x.details_.year;
  details_.julian_date = x.details_.julian_date;
  return *this;
}

date::~date() {
}

bool
operator==(const date &a, const date &b) {
  return a.details_.julian_date == b.details_.julian_date;
}

bool
operator!=(const date &a, const date &b) {
  return a.details_.julian_date != b.details_.julian_date;
}

date
operator+(const date &a, const date &b) {
  return date(a.details_.julian_date + b.details_.julian_date);
}

date
operator+(const date &a, int days) {
  return date(a.details_.julian_date + days);
}

date
operator-(const date &a, const date &b) {
  return date(a.details_.julian_date - b.details_.julian_date);
}

date
operator-(const date &a, int days) {
  return date(a.details_.julian_date - days);
}

bool
operator<(const date &a, const date &b) {
  return a.details_.julian_date < b.details_.julian_date;
}

bool
operator>(const date &a, const date &b) {
  return !(a < b || a == b);
}

bool
operator<=(const date &a, const date &b) {
  return a < b || a == b;
}

bool
operator>=(const date &a, const date &b) {
  return !(a < b);
}

std::ostream &
operator<<(std::ostream &lhs, const date &rhs) {
  lhs << rhs.details_.day << "." << rhs.details_.month << "." << rhs.details_.year;
  return lhs;
}

void
date::set(const char *datestr, const char *format) {
  struct tm t;
  strptime(datestr, format, &t);
  details_.day = t.tm_mday;
  details_.month = t.tm_mon + 1;
  details_.year = t.tm_year + 1900;
  details_.julian_date = gc2jd(details_.day, details_.month, details_.year);
}

void
date::set(int day, int month, int year) {
  details_.day = day;
  details_.month = month;
  details_.year = year;
  details_.julian_date = gc2jd(details_.day, details_.month, details_.year);
}

bool date::is_leapyear(int year)
{
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}


bool date::is_valid_date(int year, int month, int day)
{
  bool is_leap = date::is_leapyear(year);
  if (month < 1 || month > 12) {
    return false;
  }
  if (month == 2 && (month_days[month-1] + (is_leap ? 1 : 0) < day || day < 1)) {
    return false;
  } else if (month_days[month-1] < day || day < 1) {
    return false;
  }
  return true;
}


bool date::is_daylight_saving(int year, int month, int day)
{
  /*
   * Calculate day of week in proleptic Gregorian calendar. Sunday == 0.
   */
  int adjustment = (14 - month) / 12;
  int mm = month + 12 * adjustment - 2;
  int yy = year - adjustment;

  // calculate day of week
  int day_of_week = (day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
  //January, february, and december are out.
  if (month < 3 || month > 11) { return false; }
  //April to October are in
  if (month > 3 && month < 11) { return true; }
  int previousSunday = day - day_of_week;
  //In march, we are DST if our previous sunday was on or after the 8th.
  if (month == 3) { return previousSunday >= 8; }
  //In november we must be before the first sunday to be dst.
  //That means the previous sunday must be before the 1st.
  return previousSunday <= 0;
}

int
date::day() const {
  return details_.day;
}

int
date::month() const {
  return details_.month;
}

int
date::year() const {
  return details_.year;
}

int
date::julian_date() const {
  return details_.julian_date;
}

int
date::difference(const date &x) const {
  return x.julian_date() - details_.julian_date;
}

}
