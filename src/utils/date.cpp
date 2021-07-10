#include "matador/utils/date.hpp"
#include "matador/utils/calendar.h"
#include "matador/utils/strptime.hpp"

#include <ctime>
#include <stdexcept>

namespace matador {

const unsigned char date::month_days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void throw_invalid_date(int d, int m, int y)
{
  if (!date::is_valid_date(y, m, d)) {
    throw std::logic_error("date isn't valid");
  }
}

date::date()
{
  // should calc now
  time_t t = time(0);
  struct tm now;
#ifdef _MSC_VER
  localtime_s(&now, &t);
#else
  localtime_r(&t, &now);
#endif
  day_ = now.tm_mday;
  month_ = now.tm_mon + 1;
  year_ = now.tm_year + 1900;
  is_daylight_saving_ = now.tm_isdst > 0;
  is_leap_year_ = date::is_leapyear(year_);
  julian_date_ = gc2jd(day_, month_, year_);
}

date::date(int julian_date)
  : julian_date_(julian_date)
{
  sync_julian_date(julian_date);
}

date::date(int day, int month, int year)
  : day_(day)
  , month_(month)
  , year_(year)
{
  sync_date(day_, month_, year_);
}

date::date(const char *stamp, const char *format)
{
  this->set(stamp, format);
}

date::date(const date &x)
  : day_(x.day_)
  , month_(x.month_)
  , year_(x.year_)
  , is_daylight_saving_(x.is_daylight_saving_)
  , is_leap_year_(x.is_leap_year_)
  , julian_date_(x.julian_date_)
{}

date &
date::operator=(const date &x) {
  day_ = x.day_;
  month_ = x.month_;
  year_ = x.year_;
  is_daylight_saving_ = x.is_daylight_saving_;
  is_leap_year_ = x.is_leap_year_;
  julian_date_ = x.julian_date_;
  return *this;
}

date &date::operator=(int julian_date)
{
  sync_julian_date(julian_date);
  return *this;
}

date::~date()
{}

bool operator==(const date &a, const date &b)
{
  return a.julian_date_ == b.julian_date_;
}

bool operator!=(const date &a, const date &b)
{
  return a.julian_date_ != b.julian_date_;
}

date& date::operator+=(int days)
{
  sync_julian_date(julian_date_ + days);
  return *this;
}

date& date::operator-=(int days)
{
  sync_julian_date(julian_date_ - days);
  return *this;
}

date& date::operator++()
{
  sync_julian_date(julian_date_ + 1);
  return *this;
}

date date::operator++(int)
{
  date d(*this);
  ++(*this);
  return d;
}

date &date::operator--()
{
  sync_julian_date(julian_date_ - 1);
  return *this;
}

date date::operator--(int)
{
  date d(*this);
  --(*this);
  return d;
}

date operator+(date a, int days)
{
  return a += days;
}

date
operator-(date a, int days)
{
  return a -= days;
}

bool
operator<(const date &a, const date &b) {
  return a.julian_date_ < b.julian_date_;
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
  lhs << rhs.day_ << "." << rhs.month_ << "." << rhs.year_;
  return lhs;
}

date date::parse(const std::string &dstr, const char *format)
{
  struct tm t;
  detail::strptime(dstr.c_str(), format, &t);
  return date(t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
}

void date::set(const char *datestr, const char *format)
{
  struct tm t;
  detail::strptime(datestr, format, &t);
  sync_date(t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
}

void date::set(int day, int month, int year)
{
  sync_date(day, month, year);
}

void date::set(int julian_date)
{
  sync_julian_date(julian_date);
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
#ifdef _WIN32
  if (_daylight == 0) {
    return false;
  }
#else
  if (daylight == 0) {
    return false;
  }
#endif

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
  if (month == 3) { return previousSunday >= 25; }
  //In november we must be before the first sunday to be dst.
  //That means the previous sunday must be before the 1st.
  return previousSunday <= 0;
}

int
date::day() const {
  return day_;
}

int
date::month() const {
  return month_;
}

int
date::year() const {
  return year_;
}


date &date::day(int d)
{
  sync_day(d);
  return *this;
}

date &date::month(int m)
{
  sync_month(m);
  return *this;
}

date &date::year(int y)
{
  sync_year(y);
  return *this;
}


bool date::is_leapyear() const
{
  return is_leapyear(year_);
}

bool date::is_daylight_saving() const
{
  return is_daylight_saving(year_, month_, day_);
}

int
date::julian_date() const
{
  return julian_date_;
}

int
date::difference(const date &x) const {
  return x.julian_date() - julian_date_;
}


void date::sync_day(int d)
{
  sync_date(d, month_, year_);
}

void date::sync_month(int m)
{
  sync_date(day_, m, year_);
}

void date::sync_year(int y)
{
  sync_date(day_, month_, y);
}

void date::sync_date(int d, int m, int y)
{
  throw_invalid_date(d, m, y);
  day_ = d;
  month_ = m;
  year_ = y;
  // calculate julian date
  is_leap_year_ = is_leapyear(year_);
  is_daylight_saving_ = is_daylight_saving(year_, month_, day_);
  julian_date_ = gc2jd(day_, month_, year_);
}

void date::sync_julian_date(int juliandate)
{
  julian_date_ = juliandate;
  jd2gc(juliandate, day_, month_, year_);
  is_leap_year_ = is_leapyear(year_);
  is_daylight_saving_ = is_daylight_saving(year_, month_, day_);
}

}
