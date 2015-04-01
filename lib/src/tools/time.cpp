#include "tools/time.hpp"

#include <stdexcept>
#include <cstring>
#include <cmath>
#include <vector>

#include <sys/time.h>

namespace oos {

//const char *time::default_format = "%FT%T.SSSSS%z";

void throw_invalid_time(int h, int m, int s, long ms)
{
  if (!time::is_valid_time(h, m, s, ms)) {
    throw std::logic_error("time isn't valid");
  }
}

time::time()
{
  if (gettimeofday(&time_, 0) != 0) {
    throw std::logic_error("couldn' get time of day");
  }
  localtime_r(&time_.tv_sec, &tm_);
}

time::time(time_t t)
{
  set(t, 0);
}

time::time(struct timeval tv)
{
  set(tv);
}

time::time(int year, int month, int day, int hour, int min, int sec, long millis)
{
  set(year, month, day, hour, min, sec, millis);
}

//time::time(uint64_t microseconds)
//{
//  time_.tv_sec = microseconds / 1000000;
//  time_.tv_usec = microseconds % 1000000;
//  localtime_r(&time_.tv_sec, &tm_);
//}

time::time(const time &x)
  : time_(x.time_)
  , tm_(x.tm_)
{}

time &time::operator=(const time &x)
{
  time_ = x.time_;
  tm_ = x.tm_;
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

bool time::is_valid_time(int hour, int min, int sec, long millis) {
  if (hour < 0 || hour > 23) {
    return false;
  }
  if (min < 0 || min > 59) {
    return false;
  }
  if (sec < 0 || sec > 59) {
    return false;
  }
  if (millis < 0 || millis > 9999) {
    return false;
  }
  return true;
}

time time::parse(const std::string &tstr, const char *format)
{
  /*
  * find the %f format token
  * and split the string to parse
  */
  const char *pch = strstr(format, "%f");

  std::string part(format, (pch ? pch-format : strlen(format)));
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));
  const char *endptr = strptime(tstr.c_str(), part.c_str(), &tm);
  unsigned long usec = 0;
  if (endptr == nullptr && pch != nullptr) {
    // parse error
    throw std::logic_error("error parsing time");
  } else if (pch != nullptr) {
    char *next;
    usec = std::strtoul(endptr, &next, 10);
    // calculate precision
    unsigned digits = next - endptr;
    usec *= (unsigned long)pow(10.0, 6 - digits);
    if ((size_t)(next - format) != strlen(format)) {
      // still time string to parse
      strptime(next, pch+2, &tm);
    }
  }

  tm.tm_isdst = date::is_daylight_saving(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
  struct timeval tv;
  tv.tv_sec = mktime(&tm);
  tv.tv_usec = usec;
  return oos::time(tv);
}

void time::set(int year, int month, int day, int hour, int min, int sec, long millis)
{
  throw_invalid_date(day, month, year);
  throw_invalid_time(hour, min, sec, millis);

  time_t rawtime;
  ::time(&rawtime);

  struct tm *t = localtime(&rawtime);
  t->tm_year = year - 1900;
  t->tm_mon = month - 1;
  t->tm_mday = day;
  t->tm_hour = hour;
  t->tm_min = min;
  t->tm_sec = sec;
  t->tm_isdst = date::is_daylight_saving(year, month, day);

  this->time_.tv_sec = mktime(t);
  this->time_.tv_usec = millis * 1000;
  localtime_r(&this->time_.tv_sec, &this->tm_);
}

void time::set(time_t t, long millis)
{
  time_.tv_sec = t;
  time_.tv_usec = millis * 1000;
  localtime_r(&time_.tv_sec, &tm_);
}

void time::set(const date &d)
{
  set(d.year(), d.month(), d.day(), 0, 0, 0, 0);
}

void time::set(timeval tv)
{
  time_ = tv;
  localtime_r(&time_.tv_sec, &tm_);
}

int time::year() const
{
  return tm_.tm_year + 1900;
}

int time::month() const
{
  return tm_.tm_mon + 1;
}

int time::day() const
{
  return tm_.tm_mday;
}

int time::hour() const
{
   return tm_.tm_hour;
}

int time::minute() const
{
  return tm_.tm_min;
}

int time::second() const
{
  return tm_.tm_sec;
}

int time::milli_second() const
{
  return (int)(time_.tv_usec / 1000);
}

time &time::year(int y)
{
  sync_year(y);
  return *this;
}

time &time::month(int m)
{
  sync_month(m);
  return *this;
}

time &time::day(int d)
{
  sync_day(d);
  return *this;
}

time &time::hour(int h)
{
  sync_hour(h);
  return *this;
}

time &time::minute(int m)
{
  sync_minute(m);
  return *this;
}

time &time::second(int s)
{
  sync_second(s);
  return *this;
}

time &time::milli_second(int ms)
{
  sync_milli_second(ms);
  return *this;
}

int time::day_of_week() const
{
  return tm_.tm_wday;
}

int time::day_of_year() const
{
  return tm_.tm_yday;
}

bool time::is_leapyear() const
{
  return date::is_leapyear(tm_.tm_year + 1900);
}

bool time::is_daylight_saving() const
{
  return tm_.tm_isdst == 1;
}

struct timeval time::get_timeval() const
{
  return time_;
}

struct tm time::get_tm() const
{
  return tm_;
}

//std::string time::format(const char *f, tz_t tz) const
//{
//  /*
//   * find '%S.S' in format string
//   * if found check if ".S{1-5}" are
//   * the next characters. They are
//   * interpreted as fraction of a second
//   */
//  const char *m = strstr(f, "%S.S");
//  if (m == 0) {
//    m = strstr(f, "%T.S");
//  }
//  if (m != 0) {
//    /*
//     * found second token with
//     * at least one position after
//     * decimal point; find last one
//     */
//    size_t len = strlen(f);
//    // index of first S
//    size_t i = m - f + 3;
//    // calculate precision
//    while (i < len) {
//      ++i;
//      if (f[i] != 'S') {
//        break;
//      }
//    }
//    /*
//     * generate the first half of the
//     * time, add the precision digits
//     * and add the second half of the
//     * time
//     */
//    char fstr[64];
//    strncpy(fstr, f, m - f + 3);
//    fstr[m - f + 3] = '\0';
//    // generate first half
//    char first[64];
//    if (tz == local) {
//      strftime(first, 64, fstr, localtime(&time_.tv_sec));
//    } else {
//      strftime(first, 64, fstr, gmtime(&time_.tv_sec));
//    }
//    strcpy(fstr, m + 3 + i - (m - f + 3));
//    // generate second half
//    char second[64];
//    memset(second, 0, 64);
//    if (tz == local) {
//      strftime(second, 64, fstr, localtime(&time_.tv_sec));
//    } else {
//      strftime(second, 64, fstr, gmtime(&time_.tv_sec));
//    }
//    // generate time with precision
//    char buf[128];
//    memset(buf, 0, 128);
//    // calculate precision
//    char digits[7];
//    memset(digits, 0, 7);
//    sprintf(digits, "%.6ld", time_.tv_usec);
//    digits[i - (m - f + 3)] = '\0';
//    sprintf(buf, "%s%s%s", first, digits, second);
//    return buf;
//  } else {
//    char buf[64];
//    if (tz == local) {
//      strftime(buf, 64, f, localtime(&time_.tv_sec));
//    } else {
//      strftime(buf, 64, f, gmtime(&time_.tv_sec));
//    }
//    return buf;
//  }
//}

//void time::parse(const char *f, const std::string &ts)
//{
//  /*
//   * find '%S.S' in format string
//   * if found check if ".S{1-5}" are
//   * the next characters. They are
//   * interpreted as fraction of a second
//   */
//
//  struct tm t;
//  memset(&t, 0, sizeof(t));
//
//  const char *m = strstr(f, "%S.S");
//  if (m == 0) {
//    m = strstr(f, "%T.S");
//  }
//  if (m != 0) {
//    /*
//     * found second token with
//     * at least one position after
//     * decimal point; find last one
//     */
//    size_t len = strlen(f);
//    // index of first S
//    size_t i = m - f + 3;
//    // calculate precision
//    while (i < len) {
//      ++i;
//      if (f[i] != 'S') {
//        break;
//      }
//    }
//    /*
//     * extract the first part of the
//     * time, cut out the precision digits
//     * and parse the resulting time
//     * handle the fraction digits seperatly
//     */
//    char fstr[64];
//    memset(fstr, 0, 64);
//    strncpy(fstr, f, m - f + 3);
//    fstr[m - f + 3] = '\0';
//    // generate first half
//    // to get the beginning of the fraction part
//    char *next = strptime(ts.c_str(), fstr, &t);
//    char tsstr[256];
//    memset(tsstr, 0, 256);
//    strncpy(tsstr, ts.c_str(), next-ts.c_str());
//    tsstr[next-ts.c_str()] = '\0';
//    // calculate the length of the
//    // franction and the value
//    char frac[16];
//    memset(frac, 0, 16);
//    strncpy(frac, next, i - (m - f + 3));
//    frac[i - (m - f + 3)] = '\0';
//    // cut out the fraction format
//    strcpy(fstr+strlen(fstr), m + 3 + i - (m - f + 3));
//    // cut out the fraction
//    strcpy(tsstr+strlen(tsstr), next+i - (m - f + 3));
//    // parse string
//    strptime(tsstr, fstr, &t);
//    // set new time
//    time_.tv_sec = mktime(&t);
//    // convert fraction
//    sscanf(frac, "%ld", &time_.tv_usec);
//    // calculate precision
//    time_.tv_usec = time_.tv_usec * (int)pow(10.0, 6 - strlen(frac));
//  } else {
//    strptime(ts.c_str(), f, &t);
//    time_.tv_sec = mktime(&t);
//    time_.tv_usec = 0;
//  }
//}

date time::to_date() const
{
  return oos::date(tm_.tm_mday, tm_.tm_mon + 1,  tm_.tm_year + 1900);
}

std::ostream& operator<<(std::ostream &out, const time &/*x*/)
{
//  out << x.str();
  return out;
}

//std::istream& operator>>(std::istream &in, time &x)
//{
//  std::string time_string;
//  in >> time_string;
//  x.parse("%FT%T.SSSSS%z", time_string);
//  return in;
//}

void time::sync_day(int d)
{
  sync_time(tm_.tm_year + 1900, tm_.tm_mon + 1, d, tm_.tm_hour, tm_.tm_min, tm_.tm_sec, time_.tv_usec / 1000);
}

void time::sync_month(int m)
{
  sync_time(tm_.tm_year + 1900, m, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec, time_.tv_usec / 1000);
}

void time::sync_year(int y)
{
  sync_time(y, tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec, time_.tv_usec / 1000);
}

void time::sync_hour(int h)
{
  sync_time(tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday, h, tm_.tm_min, tm_.tm_sec, time_.tv_usec / 1000);
}

void time::sync_minute(int m)
{
  sync_time(tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, m, tm_.tm_sec, time_.tv_usec / 1000);
}

void time::sync_second(int s)
{
  sync_time(tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, s, time_.tv_usec / 1000);
}

void time::sync_milli_second(int ms)
{
  sync_time(tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec, ms);
}

void time::sync_time(int y, int m, int d, int h, int min, int s, long ms)
{
  set(y, m, d, h, min, s, ms);
}

}
