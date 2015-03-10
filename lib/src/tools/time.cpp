#include "tools/time.hpp"

#include <stdexcept>
#include <cstring>
#include <cmath>
#include <iostream>

#include <sys/time.h>

namespace oos {

//const char *time::default_format = "%FT%T.SSSSS%z";

unsigned char month_days[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

time::time()
{
  if (gettimeofday(&time_, 0) != 0) {
    throw std::logic_error("couldn' get time of day");
  }
}

time::time(time_t t)
{
  time_.tv_sec = t;
  time_.tv_usec = 0;
}

time::time(int year, int month, int day, int hour, int min, int sec, long millis)
{
  if (!is_valid_date(year, month, day)) {
    throw std::logic_error("invalid date");
  }

  if (is_daylight_saving(year, month, day)) {
  }
  struct tm t;
  t.tm_year = year - 1900;
  t.tm_mon = month - 1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = min;
  t.tm_sec = sec;

  time_.tv_sec = mktime(&t);
  time_.tv_usec = millis;
}

time::time(uint64_t microseconds)
{
  time_.tv_sec = microseconds / 1000000;
  time_.tv_usec = microseconds % 1000000;
}

time::time(const time &x)
  : time_(x.time_)
{}

time &time::operator=(const time &x)
{
  time_ = x.time_;
  return *this;
}

time::~time()
{
}

bool time::operator==(const time &x) const
{
  return time_.tv_sec == x.time_.tv_sec &&
    time_.tv_usec == x.time_.tv_usec;
}

bool time::operator!=(const time &x) const
{
  return !(*this == x);
}

bool time::operator<(const time &x) const
{
  return timercmp(&time_, &x.time_, <);
}

bool time::operator<=(const time &x) const
{
  return *this == x || *this < x;
}

bool time::operator>(const time &x) const
{
  return !(*this <= x);
}

bool time::operator>=(const time &x) const
{
  return !(*this < x);
}

time time::now()
{
  return time();
}


//std::string time::str() const
//{
//  return format("%FT%T.SSSSS%z", utc);
//}
//
//std::string time::to_local(const char *f) const
//{
//  return format(f, local);
//}
//
//std::string time::to_utc(const char *f) const
//{
//  return format(f, utc);
//}

bool time::is_leapyear(int year)
{
  return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}


bool time::is_valid_date(int year, int month, int day)
{
  bool is_leap = time::is_leapyear(year);
  if (month < 1 || month > 12) {
    return false;
  }
  if (month == 1 && (month_days[month] + (is_leap ? 1 : 0) < day || day < 1)) {
    return false;
  } else if (month_days[month] < day || day < 1) {
    return false;
  }
  return true;
}


bool time::is_daylight_saving(int year, int month, int day)
{
  /* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
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

int time::year() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_year + 1900;
}

int time::month() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_mon + 1;
}

int time::day() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_mday;
}

int time::day_of_week() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_wday;
}

int time::day_of_year() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_yday;
}

bool time::is_leapyear() const
{
  struct tm *t = localtime(&time_.tv_sec);
  int year = t->tm_year + 1900;
  return time::is_leapyear(year);
}

bool time::is_daylight_saving() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_isdst == 1;
}

int time::hour() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_hour;
}

int time::minute() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_min;
}

int time::second() const
{
  struct tm *t = localtime(&time_.tv_sec);
  return t->tm_sec;
}

int time::milli_second() const
{
  return time_.tv_usec;
}

//struct timeval time::operator()() const
//{
//  return time_;
//}

std::string time::format(const char *f, tz_t tz) const
{
  /*
   * find '%S.S' in format string
   * if found check if ".S{1-5}" are
   * the next characters. They are
   * interpreted as fraction of a second
   */
  const char *m = strstr(f, "%S.S");
  if (m == 0) {
    m = strstr(f, "%T.S");
  }
  if (m != 0) {
    /*
     * found second token with
     * at least one position after
     * decimal point; find last one
     */
    size_t len = strlen(f);
    // index of first S
    size_t i = m - f + 3;
    // calculate precision
    while (i < len) {
      ++i;
      if (f[i] != 'S') {
        break;
      }
    }
    /*
     * generate the first half of the
     * time, add the precision digits
     * and add the second half of the
     * time
     */
    char fstr[64];
    strncpy(fstr, f, m - f + 3);
    fstr[m - f + 3] = '\0';
    // generate first half
    char first[64];
    if (tz == local) {
      strftime(first, 64, fstr, localtime(&time_.tv_sec));
    } else {
      strftime(first, 64, fstr, gmtime(&time_.tv_sec));
    }
    strcpy(fstr, m + 3 + i - (m - f + 3));
    // generate second half
    char second[64];
    memset(second, 0, 64);
    if (tz == local) {
      strftime(second, 64, fstr, localtime(&time_.tv_sec));
    } else {
      strftime(second, 64, fstr, gmtime(&time_.tv_sec));
    }
    // generate time with precision
    char buf[128];
    memset(buf, 0, 128);
    // calculate precision
    char digits[7];
    memset(digits, 0, 7);
    sprintf(digits, "%.6ld", time_.tv_usec);
    digits[i - (m - f + 3)] = '\0';
    sprintf(buf, "%s%s%s", first, digits, second);
    return buf;
  } else {
    char buf[64];
    if (tz == local) {
      strftime(buf, 64, f, localtime(&time_.tv_sec));
    } else {
      strftime(buf, 64, f, gmtime(&time_.tv_sec));
    }
    return buf;
  }
}

void time::parse(const char *f, const std::string &ts)
{
  /*
   * find '%S.S' in format string
   * if found check if ".S{1-5}" are
   * the next characters. They are
   * interpreted as fraction of a second
   */

  struct tm t;
  memset(&t, 0, sizeof(t));

  const char *m = strstr(f, "%S.S");
  if (m == 0) {
    m = strstr(f, "%T.S");
  }
  if (m != 0) {
    /*
     * found second token with
     * at least one position after
     * decimal point; find last one
     */
    size_t len = strlen(f);
    // index of first S
    size_t i = m - f + 3;
    // calculate precision
    while (i < len) {
      ++i;
      if (f[i] != 'S') {
        break;
      }
    }
    /*
     * extract the first part of the
     * time, cut out the precision digits
     * and parse the resulting time
     * handle the fraction digits seperatly
     */
    char fstr[64];
    memset(fstr, 0, 64);
    strncpy(fstr, f, m - f + 3);
    fstr[m - f + 3] = '\0';
    // generate first half
    // to get the beginning of the fraction part
    char *next = strptime(ts.c_str(), fstr, &t);
    char tsstr[256];
    memset(tsstr, 0, 256);
    strncpy(tsstr, ts.c_str(), next-ts.c_str());
    tsstr[next-ts.c_str()] = '\0';
    // calculate the length of the
    // franction and the value
    char frac[16];
    memset(frac, 0, 16);
    strncpy(frac, next, i - (m - f + 3));
    frac[i - (m - f + 3)] = '\0';
    // cut out the fraction format
    strcpy(fstr+strlen(fstr), m + 3 + i - (m - f + 3));
    // cut out the fraction
    strcpy(tsstr+strlen(tsstr), next+i - (m - f + 3));
    // parse string
    strptime(tsstr, fstr, &t);
    // set new time
    time_.tv_sec = mktime(&t);
    // convert fraction
    sscanf(frac, "%ld", &time_.tv_usec);
    // calculate precision
    time_.tv_usec = time_.tv_usec * (int)pow(10.0, 6 - strlen(frac));
  } else {
    strptime(ts.c_str(), f, &t);
    time_.tv_sec = mktime(&t);
    time_.tv_usec = 0;
  }
}

std::ostream& operator<<(std::ostream &out, const time &/*x*/)
{
//  out << x.str();
  return out;
}

std::istream& operator>>(std::istream &in, time &x)
{
  std::string time_string;
  in >> time_string;
  x.parse("%FT%T.SSSSS%z", time_string);
  return in;
}

}
