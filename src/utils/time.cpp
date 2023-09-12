#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/strptime.hpp"

#include <stdexcept>
#include <cstring>
#include <cmath>
#include <chrono>

namespace matador {

namespace detail {

#ifdef _MSC_VER
/* FILETIME of Jan 1 1970 00:00:00. */
  static const unsigned __int64 epoch = ((unsigned __int64)116444736000000000ULL);
#endif
}

void localtime(const time_t &in, struct tm &out)
{
#if defined(__unix__)
  localtime_r(&in, &out);
#elif defined(_MSC_VER)
  errno_t err = localtime_s(&out, &in);
#else
  static std::mutex mtx;
  std::lock_guard<std::mutex> lock(mtx);
  out = *std::localtime(&in);
#endif
}

void gmtime(const time_t &in, tm &out)
{
#ifdef _MSC_VER
  errno_t err = gmtime_s(&out, &in);
#else
  gmtime_r(&in, &out);
#endif
}

size_t strftime(char *buffer, size_t size, const char *format, const time_info &ti)
{
//  struct tm timeinfo = {};
//  localtime(tv->tv_sec, timeinfo);
  // find position of "%f" for milliseconds
  const char *fpos = strstr(format, "%f");
  if (fpos != nullptr) {
    // split format string (exclude %f)
    size_t len = fpos - format;
    if (len > 64) {
      throw std::logic_error("couldn't format date string");
    }
    char first_part[64];
#ifdef _MSC_VER
    strncpy_s(first_part, 64, format, len);
#else
    strncpy(first_part, format, len);
#endif
    first_part[len] = '\0';
    auto s = ::strftime(buffer, size, first_part, &ti.timestamp);
    if (s == 0) {
      throw std::logic_error("couldn't format date string");
    }

    s += snprintf(buffer + s, size - s, "%03u", ti.milliseconds);

    const char *last_part = fpos + 2;
    // check if last part is valid
    if (last_part < format + strlen(format)) {
      s = ::strftime(buffer + s, size - s, last_part, &ti.timestamp);
      if (s == 0) {
        throw std::logic_error("couldn't format date string");
      }
    }
    return s;
  } else {
    // "%f" not found; call usual strftime
    auto s = ::strftime(buffer, size, format, &ti.timestamp);
    if (s == 0) {
      throw std::logic_error("couldn't format date string");
    }
    return s;
  }
  return 0;
}

void gettimeofday(time_info &ti)
{
  auto now=std::chrono::system_clock::now();
  auto milliseconds= std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
  ti.milliseconds = (milliseconds.count()%1000);
  ti.seconds_since_epoch = milliseconds.count()/1000;
  localtime(ti.seconds_since_epoch, ti.timestamp);
}

void throw_invalid_time(int h, int m, int s, unsigned int millis)
{
  if (!time::is_valid_time(h, m, s, millis)) {
    throw std::logic_error("time isn't valid");
  }
}

time::time()
{
  matador::gettimeofday(time_info_);
  localtime(time_info_.seconds_since_epoch, time_info_.timestamp);
}

time::time(time_t t)
{
  set(t, 0);
}

time::time(const time_info &ti)
: time_info_(ti)
{}

time::time(int year, int month, int day, int hour, int min, int sec, long millis)
{
  set(year, month, day, hour, min, sec, millis);
}

bool time::operator==(const time &x) const
{
  return time_info_.seconds_since_epoch == x.time_info_.seconds_since_epoch &&
    time_info_.milliseconds == x.time_info_.milliseconds;
}

bool time::operator!=(const time &x) const
{
  return !(*this == x);
}

bool time::operator<(const time &x) const
{
  return time_info_.seconds_since_epoch < x.time_info_.seconds_since_epoch ||
         (time_info_.seconds_since_epoch == x.time_info_.seconds_since_epoch && time_info_.milliseconds < x.time_info_.milliseconds);
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
  return time{};
}

time time::to_utc() const
{
  time_info utc;
  gmtime(time_info_.seconds_since_epoch, utc.timestamp);
  utc.seconds_since_epoch = mktime(&utc.timestamp);
  utc.milliseconds = time_info_.milliseconds;
  return time{utc};
}

bool time::is_valid_time(int hour, int min, int sec, unsigned int millis) {
  if (hour < 0 || hour > 23) {
    return false;
  }
  if (min < 0 || min > 59) {
    return false;
  }
  if (sec < 0 || sec > 59) {
    return false;
  }
  return millis <= 9999;
}

time time::parse(const std::string &tstr, const char *format)
{
  auto ti = parse_time_string(tstr, format);
  return time{ti};
}

void time::set(int year, int month, int day, int hour, int min, int sec, unsigned int millis)
{
  throw_invalid_date(day, month, year);
  throw_invalid_time(hour, min, sec, millis);

  struct tm temp_tm = {};
  temp_tm.tm_year = year - 1900;
  temp_tm.tm_mon = month - 1;
  temp_tm.tm_mday = day;
  temp_tm.tm_hour = hour;
  temp_tm.tm_min = min;
  temp_tm.tm_sec = sec;
  // set 'tm_isdst' to -1 to let mktime() determine correct value
  temp_tm.tm_isdst = -1;

  time_info_.seconds_since_epoch = std::mktime(&temp_tm);
  time_info_.milliseconds = millis;
  localtime(time_info_.seconds_since_epoch, time_info_.timestamp);
}

void time::set(const char *timestr, const char *format)
{
  set(parse_time_string(timestr, format));
}

void time::set(time_t t, unsigned int millis)
{
  time_info_.seconds_since_epoch = t;
  time_info_.milliseconds = millis;
  localtime(time_info_.seconds_since_epoch, time_info_.timestamp);
}

void time::set(const date &d)
{
  set(d.year(), d.month(), d.day(), 0, 0, 0, 0);
}

void time::set(const time_info &ti)
{
  time_info_ = ti;
  localtime(time_info_.seconds_since_epoch, time_info_.timestamp);
}

int time::year() const
{
  return time_info_.timestamp.tm_year + 1900;
}

int time::month() const
{
  return time_info_.timestamp.tm_mon + 1;
}

int time::day() const
{
  return time_info_.timestamp.tm_mday;
}

int time::hour() const
{
   return time_info_.timestamp.tm_hour;
}

int time::minute() const
{
  return time_info_.timestamp.tm_min;
}

int time::second() const
{
  return time_info_.timestamp.tm_sec;
}

unsigned int time::milli_second() const
{
  return time_info_.milliseconds;
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
  return time_info_.timestamp.tm_wday;
}

int time::day_of_year() const
{
  return time_info_.timestamp.tm_yday;
}

bool time::is_leap_year() const
{
  return date::is_leapyear(time_info_.timestamp.tm_year + 1900);
}

bool time::is_daylight_saving() const
{
  return time_info_.timestamp.tm_isdst == 1;
}

time_info time::get_time_info() const
{
  return time_info_;
}

struct tm time::get_tm() const
{
  return time_info_.timestamp;
}

date time::to_date() const
{
  return {time_info_.timestamp.tm_mday, time_info_.timestamp.tm_mon + 1,  time_info_.timestamp.tm_year + 1900};
}

std::ostream& operator<<(std::ostream &out, const time &x)
{
  out << to_string(x);
  return out;
}

void time::sync_day(int d)
{
  sync_time(time_info_.timestamp.tm_year + 1900, time_info_.timestamp.tm_mon + 1, d, time_info_.timestamp.tm_hour, time_info_.timestamp.tm_min, time_info_.timestamp.tm_sec, time_info_.milliseconds);
}

void time::sync_month(int m)
{
  sync_time(time_info_.timestamp.tm_year + 1900, m, time_info_.timestamp.tm_mday, time_info_.timestamp.tm_hour, time_info_.timestamp.tm_min, time_info_.timestamp.tm_sec, time_info_.milliseconds);
}

void time::sync_year(int y)
{
  sync_time(y, time_info_.timestamp.tm_mon + 1, time_info_.timestamp.tm_mday, time_info_.timestamp.tm_hour, time_info_.timestamp.tm_min, time_info_.timestamp.tm_sec, time_info_.milliseconds);
}

void time::sync_hour(int h)
{
  sync_time(time_info_.timestamp.tm_year + 1900, time_info_.timestamp.tm_mon + 1, time_info_.timestamp.tm_mday, h, time_info_.timestamp.tm_min, time_info_.timestamp.tm_sec, time_info_.milliseconds);
}

void time::sync_minute(int m)
{
  sync_time(time_info_.timestamp.tm_year + 1900, time_info_.timestamp.tm_mon + 1, time_info_.timestamp.tm_mday, time_info_.timestamp.tm_hour, m, time_info_.timestamp.tm_sec, time_info_.milliseconds);
}

void time::sync_second(int s)
{
  sync_time(time_info_.timestamp.tm_year + 1900, time_info_.timestamp.tm_mon + 1, time_info_.timestamp.tm_mday, time_info_.timestamp.tm_hour, time_info_.timestamp.tm_min, s, time_info_.milliseconds);
}

void time::sync_milli_second(int ms)
{
  sync_time(time_info_.timestamp.tm_year + 1900, time_info_.timestamp.tm_mon + 1, time_info_.timestamp.tm_mday, time_info_.timestamp.tm_hour, time_info_.timestamp.tm_min, time_info_.timestamp.tm_sec, ms);
}

void time::sync_time(int y, int m, int d, int h, int min, int s, unsigned int millis)
{
  set(y, m, d, h, min, s, millis);
}

time_info time::parse_time_string(const std::string &tstr, const char *format)
{
  /*
  * find the %f format token
  * and split the string to parse
  */
  const char *pch = strstr(format, "%f");

  std::string part(format, (pch ? pch-format : strlen(format)));
  time_info ti{};
  memset(&ti.timestamp, 0, sizeof(struct tm));
  const char *endptr = detail::strptime(tstr.c_str(), part.c_str(), &ti.timestamp);
  unsigned int usec = 0;
  if (endptr == nullptr && pch != nullptr) {
    // parse error
    throw std::logic_error("error parsing time: " + tstr);
  } else if (pch != nullptr) {
    char *next;
    usec = std::strtoul(endptr, &next, 10);
    // calculate precision
    auto digits = (unsigned int) (next - endptr);
    usec *= (unsigned long)pow(10.0, 3 - digits);
    if ((size_t)(next - format) != strlen(format)) {
      // still time string to parse
      detail::strptime(next, pch+2, &ti.timestamp);
    }
  }

  // set 'tm_isdst' to -1 to let mktime() determine correct value
  ti.timestamp.tm_isdst = -1;
  ti.milliseconds = usec;
  ti.seconds_since_epoch = mktime(&ti.timestamp);
  
  return ti;
}

}
