#ifndef TIME_HPP
#define TIME_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include "tools/date.hpp"

#include <ctime>
#include <cstdint>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif

namespace oos {

/**
 * @class time
 *
 * @brief Simple time class with milliseconds
 *
 * This class provides a simple interface
 * for time representation. It is possible to
 * create a time with millisecond precision.
 *
 * Addition and subtraction of times is possible
 * as well as parsing and formatting.
 */
class OOS_API time
{
public:
  /*! timezone enum */
  enum tz_t {
    local, /*!< use local timezone */
    utc    /*!< use utc timezone */
  };

  /**
   * Creates a new time with
   * time now
   */
  time();

  /**
   * Create a time from time_t
   *
   * @param t time_t value to create time from
   */
  explicit time(time_t t);

  /**
   * Create a time from a timeval struct
   *
   * @param t timeval value to create time from
   */
  explicit time(struct timeval tv);

  /**
   * Create a time from discret values
   *
   * @param year Year value of time
   * @param month Month value of time
   * @param day Day value of time
   * @param hour Hour part of time
   * @param min Minute part of time
   * @param sec Second part of time
   * @param millis Milliseconds of time (default is zero)
   */
  time(int year, int month, int day, int hour, int min, int sec, long millis = 0);

  /**
   * Create a time from another time
   *
   * @param x Other time object to be initialized from
   */
  time(const time &x);

  /**
   * Assign time from another time object
   *
   * @param x Assign time from this time object
   * @return Return reference of this
   */
  time &operator=(const time &x);

  /**
   * Destroy time
   */
  ~time();

  /**
   * Check if two times are equal
   *
   * @param x Time object to compare with
   * @return True if times are equal
   */
  bool operator==(const time &x) const;

  /**
   * Check if two times are not equal
   *
   * @param x Time object to compare with
   * @return True if times are not equal
   */
  bool operator!=(const time &x) const;

  /**
   * Check if time is less equal than other time
   *
   * @param x Time object to compare with
   * @return True if time is less equal than other time
   */
  bool operator<=(const time &x) const;

  /**
   * Check if time is less than other time
   *
   * @param x Time object to compare with
   * @return True if time is less than other time
   */
  bool operator<(const time &x) const;

  /**
   * Check if time is greater than other time
   *
   * @param x Time object to compare with
   * @return True if time is greater than other time
   */
  bool operator>(const time &x) const;

  /**
   * Check if time is greater equal than other time
   *
   * @param x Time object to compare with
   * @return True if time is greater equal than other time
   */
  bool operator>=(const time &x) const;

  static time now();

  static bool is_valid_time(int hour, int min, int sec, long millis);

  static time parse(const std::string &tstr, const char *format);

  void set(int year, int month, int day, int hour, int min, int sec, long millis);
  void set(time_t t, long millis);
  void set(const date &d);
  void set(timeval tv);

  int year() const;
  int month() const;
  int day() const;
  int hour() const;
  int minute() const;
  int second() const;
  int milli_second() const;

  time& year(int y);
  time& month(int m);
  time& day(int d);
  time& hour(int h);
  time& minute(int m);
  time& second(int s);
  time& milli_second(int ms);

  int day_of_week() const;
  int day_of_year() const;

  bool is_leapyear() const;
  bool is_daylight_saving() const;

  struct timeval get_timeval() const;
  struct tm get_tm() const;

  date to_date() const;

  friend std::ostream &operator<<(std::ostream &out, const time &x);

private:
  void sync_day(int d);
  void sync_month(int m);
  void sync_year(int y);
  void sync_hour(int h);
  void sync_minute(int m);
  void sync_second(int s);
  void sync_milli_second(int ms);
  void sync_time(int y, int m, int d, int h, int min, int s, long ms);

private:
  struct timeval time_;
  struct tm tm_;
};

}

#endif /* TIME_HPP */