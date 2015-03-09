#include "tools/date.hpp"
#include "tools/calendar.h"

#include <ctime>

namespace oos {

date::date() {
  // should calc now
  time_t t = time(0);
  struct tm *now = localtime(&t);
  day_ = now->tm_mday;
  month_ = now->tm_mon + 1;
  year_ = now->tm_year + 1900;
  julian_date_ = gc2jd(day_, month_, year_);
}

date::date(int julian_date) : julian_date_(julian_date) {
  jd2gc(julian_date_, day_, month_, year_);
}

date::date(int day, int month, int year) :
  day_(day),
  month_(month),
  year_(year) {
  // calculate julian date
  julian_date_ = gc2jd(day_, month_, year_);
}

date::date(const char *stamp, const char *format) {
  this->set(stamp, format);
}

date::date(const date &x) :
  day_(x.day_),
  month_(x.month_),
  year_(x.year_),
  julian_date_(x.julian_date_) {
}

date &
date::operator=(const date &x) {
  day_ = x.day_;
  month_ = x.month_;
  year_ = x.year_;
  julian_date_ = x.julian_date_;
  return *this;
}

date::~date() {
}

bool
operator==(const date &a, const date &b) {
  return a.julian_date_ == b.julian_date_;
}

bool
operator!=(const date &a, const date &b) {
  return a.julian_date_ != b.julian_date_;
}

date
operator+(const date &a, const date &b) {
  return date(a.julian_date_ + b.julian_date_);
}

date
operator+(const date &a, int days) {
  return date(a.julian_date_ + days);
}

date
operator-(const date &a, const date &b) {
  return date(a.julian_date_ - b.julian_date_);
}

date
operator-(const date &a, int days) {
  return date(a.julian_date_ - days);
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

void
date::set(const char *datestr, const char *format) {
  struct tm t;
  strptime(datestr, format, &t);
  day_ = t.tm_mday;
  month_ = t.tm_mon + 1;
  year_ = t.tm_year + 1900;
  julian_date_ = gc2jd(day_, month_, year_);
}

void
date::set(int day, int month, int year) {
  day_ = day;
  month_ = month;
  year_ = year;
  julian_date_ = gc2jd(day_, month_, year_);
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

int
date::julian_date() const {
  return julian_date_;
}

int
date::difference(const date &x) const {
  return x.julian_date() - julian_date_;
}

}
