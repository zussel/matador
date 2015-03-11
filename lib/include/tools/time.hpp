#ifndef TIME_HPP
#define TIME_HPP

#include <ctime>
#include <cstdint>
#include <string>

namespace oos {

class time {
public:
  enum tz_t {
    local,
    utc
  };

  static const char *default_format;

  /**
   * Creates a new time with
   * time now
   */
  time();

  explicit time(time_t t);
  time(int year, int month, int day, int hour, int min, int sec, long millis = 0);
  explicit time(uint64_t microseconds);
  time(const time &x);
  time &operator=(const time &x);
  ~time();

  bool operator==(const time &x) const;
  bool operator!=(const time &x) const;
  bool operator<=(const time &x) const;
  bool operator<(const time &x) const;
  bool operator>(const time &x) const;
  bool operator>=(const time &x) const;

  static time now();

  static bool is_valid_time(int hour, int min, int sec, int millis);

//  std::string str() const;
//
//  std::string to_local(const char *f) const;
//
//  std::string to_utc(const char *f) const;

  int year() const;
  int month() const;
  int day() const;
  int day_of_week() const;
  int day_of_year() const;

  bool is_leapyear() const;
  bool is_daylight_saving() const;

  int hour() const;
  int minute() const;
  int second() const;
  int milli_second() const;

//  struct timeval operator()() const;

  std::string format(const char *f, tz_t tz = local) const;

  void parse(const char *f, const std::string &ts);

  friend std::ostream &operator<<(std::ostream &out, const time &x);

  friend std::istream &operator>>(std::istream &in, time &x);

private:
  struct timeval time_;
  struct tm tm_;
};

}

#endif /* TIME_HPP */