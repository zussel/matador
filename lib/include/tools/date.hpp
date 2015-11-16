#ifndef DATE_HPP
#define DATE_HPP

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

#include <iostream>

namespace oos {

void throw_invalid_date(int d, int m, int y);
/**
 * @class date
 * @brief This class provides simple date representation
 *
 * This class provides simple date representation with
 * julian calendar for date calcultion.
 *
 * A date can be constructed either from
 * - a julian date
 * - year, month and day values
 * - from a date string and its format
 * - copied from another date
 */
class OOS_API date
{
public:
  static const unsigned char month_days[];

public:
  /**
   * Creates a date of now
   */
  date();

  /**
   * Creates a date from the
   * given julian date
   *
   * @param julian_date The julian date to create a date from
   */
  explicit date(int julian_date);

  /**
   * Creates a date from given year, month
   * and day
   *
   * @param day Dates day
   * @param month Dates month
   * @param year Dates year
   */
  date(int day, int month, int year);

  /**
   * Creates a date from a given date/time
   * string and a format string
   *
   * @param stamp The date/time string to parse
   * @param format The format to use for parsing
   */
  explicit date(const char *stamp, const char *format = "%d.%m.%Y");

  date(const date &x);
  date& operator=(const date &x);

  /**
   * Assign a date from julian date value
   *
   * @param julian_date The julian adte to assign from
   * @return The assigned date
   */
  date& operator=(int julian_date);

  virtual ~date();

  friend OOS_API bool operator==(const date &a, const date &b);
  friend OOS_API bool operator!=(const date &a, const date &b);

  date& operator+=(int days);
  date& operator-=(int days);

  date& operator++();
  date operator++(int);
  date& operator--();
  date operator--(int);

  friend OOS_API date operator+(date a, int days);
  friend OOS_API date operator-(date a, int days);

  friend OOS_API bool operator<(const date &a, const date &b);
  friend OOS_API bool operator>(const date &a, const date &b);
  friend OOS_API bool operator<=(const date &a, const date &b);
  friend OOS_API bool operator>=(const date &a, const date &b);

  friend OOS_API std::ostream& operator<< (std::ostream& lhs, const date& rhs);

  // sets the date from a string
  void set(const char *datestr, const char *format = "%d.%m.%Y");
  void set(int day, int month, int year);
  void set(int julian_date);

  /**
   * Returns the day part of date
   *
   * @return The day part of date.
   */
  int day() const;

  /**
   * Returns the month part of date
   *
   * @return The month part of date.
   */
  int month() const;

  /**
   * Returns the year part of date
   *
   * @return The year part of date.
   */
  int year() const;

  date& day(int d);
  date& month(int m);
  date& year(int y);

  /**
   * Returns true if dates year is
   * a leap year.
   *
   * @return True If year of date is leap year.
   */
  bool is_leapyear() const;

  /**
   * Returns true if date is in
   * daylight saving range which depends
   * one the country.
   *
   * @return True if date is in daylight saving range.
   */
  bool is_daylight_saving() const;

  /**
   * Returns true if dates year is
   * a leap year.
   *
   * @param year The year to check.
   * @return True If year of date is leap year.
   */
  static bool is_leapyear(int year);

  /**
   * Checks if given date parts are valid
   *
   * 0 < month <= 12
   * Valid day with given month
   *
   * @param year Year to check
   * @param month Month to check
   * @param day Day to check
   * @return true if date parts are valid
   */
  static bool is_valid_date(int year, int month, int day);

  /**
   * Returns true if time is in
   * daylight saving range which depends
   * one the country.
   *
   * @return True if time is in daylight saving range.
   */
  static bool is_daylight_saving(int year, int month, int day);

  /**
   * Returns the underlaying julian date
   *
   * @return The underlaying julian date
   */
  int julian_date() const;

  /**
   * Returns the difference of two
   * dates in days.
   *
   * @param x The date to calculate the difference with.
   * @return The difference in days.
   */
  virtual int difference(const date &x) const;

private:
  void sync_day(int d);
  void sync_month(int m);
  void sync_year(int y);
  void sync_date(int d, int m, int y);
  void sync_julian_date(int juliandate);

private:
  int day_;
  int month_;
  int year_;
  bool is_daylight_saving_;
  bool is_leap_year_;
  int julian_date_;
};

}

#endif /* DATE_HPP */
