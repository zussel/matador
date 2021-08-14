#ifndef TIME_HPP
#define TIME_HPP

#ifdef _MSC_VER
  #ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "matador/utils/date.hpp"

#include <ctime>
#include <cstdint>
#include <string>

#ifdef _MSC_VER
#include <winsock2.h>
#endif

namespace matador {

/**
 * Multi platform version of localtime
 *
 * @param in time_t value to be converted
 * @param out converted value
 */
OOS_UTILS_API void localtime(const time_t &in, struct tm &out);

/**
 * Multi platform version of gmtime
 *
 * @param in time_t value to be converted
 * @param out converted value
 */
OOS_UTILS_API void gmtime(const time_t &in, struct tm &out);

/**
 * Formats a given timeval struct as a string
 * within the given buffer. It is possible to format the
 * string with fractional seconds using the format
 * token '%f'.
 *
 * @param buffer Buffer to write the time string to
 * @param size Size of the buffer
 * @param format Format of the time string
 * @param tv timeval struct containing the time to format
 * @return The length of the buffer string
 */
OOS_UTILS_API int strftime(char *buffer, size_t size, const char *format, const struct timeval *tv);

/**
 * Multi platform version of gettimeofday
 *
 * @param tp Timeval struct where the result ist stored
 * @return Returns 0 on success
 */
OOS_UTILS_API int gettimeofday(struct timeval *tp);

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
class OOS_UTILS_API time
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
   * @param tv timeval value to create time from
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
  time(const time &x) = default;

  /**
   * Copy move from another time
   *
   * @param x Other time object to be move from
   */
  time(time &&x) = default;

  /**
   * Assign time from another time object
   *
   * @param x Assign time from this time object
   * @return Return reference of this
   */
  time& operator=(const time &x) = default;

  /**
   * Assignment move from another time
   *
   * @param x Assign move time from this time object
   * @return Return reference of this
   */
  time& operator=(time &&x) = default;

  /**
   * Destroy time
   */
  ~time() = default;

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

  /**
   * Gets current time.
   *
   * @return Returns current time.
   */
  static time now();

  /**
   * Checks if given time parts are valid
   *
   * 0 <= hour < 24
   * 0 <= minutes < 60
   * 0 <= seconds < 60
   * 0 <= milliseconds < 1000
   *
   * @param hour Hour to check
   * @param min Minute to check
   * @param sec Seconds to check
   * @param millis Milliseconds to check
   * @return true if time parts are valid
   */
  static bool is_valid_time(int hour, int min, int sec, long millis);

  /**
   * Parse a given time string with a valid format
   * and return a corresponding time object.
   *
   * @param tstr Time string.
   * @param format Time strings format.
   * @return A time object.
   */
  static time parse(const std::string &tstr, const char *format);

  /**
   * Sets time by its parts.
   *
   * @param year The year part of the time.
   * @param month The month part of the time.
   * @param day The day part of the time.
   * @param hour The hour part of the time.
   * @param min The minute part of the time.
   * @param sec The seconds part of the time.
   * @param millis The milliseconds part of the time.
   */
  void set(int year, int month, int day, int hour, int min, int sec, long millis);

  /**
   * Sets the date from a given date/time
   * string and a format string
   *
   * @param timestr The date/time string to parse
   * @param format The format to use for parsing
   */
  void set(const char *timestr, const char *format = "%Y-%m-%d %H:%M%S");

  /**
   * Sets the time from a given time_t and milliseconds
   *
   * @param t The time_t to set.
   * @param millis The milliseconds to set.
   */
  void set(time_t t, long millis);

  /**
   * Sets the time from a date. The hour,
   * minute, seconds and millisecond part
   * is set to zero
   *
   * @param d The date to set.
   */
  void set(const date &d);

  /**
   * Sets the time from a given timeval
   * object.
   *
   * @param tv Timeval object to set the time from.
   */
  void set(timeval tv);

  /**
   * Return the year of the time.
   *
   * @return The year part of the time.
   */
  int year() const;

  /**
   * Return the month of the time.
   *
   * @return The month part of the time.
   */
  int month() const;

  /**
   * Return the day of the time.
   *
   * @return The day part of the time.
   */
  int day() const;

  /**
   * Return the hour of the time.
   *
   * @return The hour part of the time.
   */
  int hour() const;

  /**
   * Return the minute of the time.
   *
   * @return The minute part of the time.
   */
  int minute() const;

  /**
   * Return the seconds of the time.
   *
   * @return The second part of the time.
   */
  int second() const;

  /**
   * Return the milliseconds of the time.
   *
   * @return The millisecond part of the time.
   */
  int milli_second() const;

  /**
   * Sets the year part of the time and
   * returns the time object.
   *
   * @param y The year part to set.
   * @return The time object.
   */
  time& year(int y);

  /**
   * Sets the month part of the time and
   * returns the time object.
   *
   * @param m The month part to set.
   * @return The time object.
   */
  time& month(int m);

  /**
   * Sets the day part of the time and
   * returns the time object.
   *
   * @param d The day part to set.
   * @return The time object.
   */
  time& day(int d);

  /**
   * Sets the hour part of the time and
   * returns the time object.
   *
   * @param h The hour part to set.
   * @return The time object.
   */
  time& hour(int h);

  /**
   * Sets the minute part of the time and
   * returns the time object.
   *
   * @param m The minute part to set.
   * @return The time object.
   */
  time& minute(int m);

  /**
   * Sets the second part of the time and
   * returns the time object.
   *
   * @param s The second part to set.
   * @return The time object.
   */
  time& second(int s);

  /**
   * Sets the millisecond part of the time and
   * returns the time object.
   *
   * @param ms The millisecond part to set.
   * @return The time object.
   */
  time& milli_second(int ms);


  /**
   * Returns the day of week number.
   * By now the week starts with monday
   * which is index 0 (zero).
   *
   * @return The day of week number.
   */
  int day_of_week() const;

  /**
   * Returns the day of the year
   * number. First day of the year
   * is number one (1).
   *
   * @return The day of the year number.
   */
  int day_of_year() const;

  /**
   * Returns true if times year is
   * a leap year.
   *
   * @return True If year of time is leap year.
   */
  bool is_leapyear() const;

  /**
   * Returns true if time is in
   * daylight saving range which depends
   * one the country.
   *
   * @return True if time is in daylight saving range.
   */
  bool is_daylight_saving() const;

  /**
   * Returns the underlaying timeval struct
   * representing the time.
   *
   * @return The underlaying timeval struct
   */
  struct timeval get_timeval() const;

  /**
   * Returns the time as struct tm representing
   * the time without milli seconds
   *
   * @return The time as struct tm.
   */
  struct tm get_tm() const;

  /**
   * Converts and returns the date part
   * of the time to a date objects
   *
   * @return The date part of the time as date object.
   */
  date to_date() const;

  /**
   * Writes the time to an std::ostream in
   * ISO8601 format
   *
   * @param out The stream to write to.
   * @param x The time object to be printed
   * @return Reference to the passed ostream object.
   */
  friend OOS_UTILS_API std::ostream &operator<<(std::ostream &out, const time &x);

private:
  void sync_day(int d);
  void sync_month(int m);
  void sync_year(int y);
  void sync_hour(int h);
  void sync_minute(int m);
  void sync_second(int s);
  void sync_milli_second(int ms);
  void sync_time(int y, int m, int d, int h, int min, int s, long ms);

  static timeval parse_time_string(const std::string &tstr, const char *format);

private:
  struct timeval time_ = {0,0 };
//  struct tm tm_ = {0,0,0,0,0,0,0,0,0,0,nullptr};

  std::tm tm_ = {};
};

}

#endif /* TIME_HPP */