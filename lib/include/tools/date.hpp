#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>

namespace oos {

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

  int julian_date() const;

  virtual int difference(const date &x) const;

protected:
  int day_;
  int month_;
  int year_;

  int julian_date_;
};

}

#endif /* DATE_HPP */
