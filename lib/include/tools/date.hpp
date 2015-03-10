#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>

namespace oos {

unsigned char month_days[] { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

class date {
public:
  date();
  explicit date(int julian_date);
  date(int day, int month, int year);
  date(const char *stamp, const char *format = "%d.%m.%Y");
  date(const date &x);
  date& operator=(const date &x);

  virtual ~date();

  friend bool operator==(const date &a, const date &b);
  friend bool operator!=(const date &a, const date &b);

  friend date operator+(const date &a, const date &b);
  friend date operator+(const date &a, int days);
  friend date operator-(const date &a, const date &b);
  friend date operator-(const date &a, int days);

  friend bool operator<(const date &a, const date &b);
  friend bool operator>(const date &a, const date &b);
  friend bool operator<=(const date &a, const date &b);
  friend bool operator>=(const date &a, const date &b);

  friend std::ostream& operator<< (std::ostream& lhs, const date& rhs);
  // friend std::istream& operator>> (std::istream& lhs, date& rhs);
  // sets the date from a string
  void set(const char *datestr, const char *format = "%d.%m.%Y");
  void set(int day, int month, int year);

  int day() const;
  int month() const;
  int year() const;

  static bool is_leapyear(int year);

  static bool is_valid_date(int year, int month, int day);

  static bool is_daylight_saving(int year, int month, int day);

  int julian_date() const;

  virtual int difference(const date &x) const;

private:
  struct details_t {
    details_t();
    details_t(int jd);
    details_t(int y, int m, int d);
    details_t(const details_t &x);
    int day;
    int month;
    int year;
    bool is_daylight_saving;
    bool is_leap_year;
    int julian_date;
  };

private:
  details_t details_;
};

}

#endif /* DATE_HPP */
