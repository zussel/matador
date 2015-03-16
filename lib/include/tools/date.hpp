#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>

namespace oos {

void throw_invalid_date(int d, int m, int y);

class date {
public:
  static const unsigned char month_days[];

public:
  date();
  explicit date(int julian_date);
  date(int day, int month, int year);
  explicit date(const char *stamp, const char *format = "%d.%m.%Y");
  date(const date &x);
  date& operator=(const date &x);
  date& operator=(int julian_date);

  virtual ~date();

  friend bool operator==(const date &a, const date &b);
  friend bool operator!=(const date &a, const date &b);

  date& operator+=(int days);
  date& operator-=(int days);

  date& operator++();
  date operator++(int);
  date& operator--();
  date operator--(int);

  friend date operator+(date a, int days);
  friend date operator-(date a, int days);

  friend bool operator<(const date &a, const date &b);
  friend bool operator>(const date &a, const date &b);
  friend bool operator<=(const date &a, const date &b);
  friend bool operator>=(const date &a, const date &b);

  friend std::ostream& operator<< (std::ostream& lhs, const date& rhs);
  // friend std::istream& operator>> (std::istream& lhs, date& rhs);
  // sets the date from a string
  void set(const char *datestr, const char *format = "%d.%m.%Y");
  void set(int day, int month, int year);
  void set(int julian_date);

  int day() const;
  int month() const;
  int year() const;

  date& day(int d);
  date& month(int m);
  date& year(int y);

  bool is_leapyear() const;
  bool is_daylight_saving() const;

  static bool is_leapyear(int year);

  static bool is_valid_date(int year, int month, int day);

  static bool is_daylight_saving(int year, int month, int day);

  int julian_date() const;

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
