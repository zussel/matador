#ifndef MATADOR_DATE_HPP
#define MATADOR_DATE_HPP

#include <optional>
#include <tuple>

namespace matatdor {
class date {
public:
  class year_type {
  public:
    explicit constexpr year_type(const int v) : value_(v) {}

    [[nodiscard]] constexpr int value() const { return value_; }

  private:
    int value_;
  };

  class month_type {
  public:
    explicit constexpr month_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  class day_type {
  public:
    explicit constexpr day_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  date();

  [[nodiscard]] static constexpr std::optional<date> make(const year_type y, const month_type m, const day_type d) {
    if (!is_valid(y, m, d)) {
      return std::nullopt;
    }

    return date{y, m, d};
  }

  [[nodiscard]] constexpr year_type year() const { return year_; }
  [[nodiscard]] constexpr month_type month() const { return month_; }
  [[nodiscard]] constexpr day_type day() const { return day_; }

  [[nodiscard]] static constexpr bool is_leap_year(year_type y) {
    const int yy = y.value();

    return yy % 4 == 0 && (yy % 100 != 0 || yy % 400 == 0);
  }
  [[nodiscard]] static constexpr unsigned days_in_month(year_type y, month_type m) {
    switch (m.value()) {
    case 1:
      return 31;
    case 2:
      return is_leap_year(y) ? 29u : 28u;
    case 3:
      return 31;
    case 4:
      return 30;
    case 5:
      return 31;
    case 6:
      return 30;
    case 7:
    case 8:
      return 31;
    case 9:
      return 30;
    case 10:
      return 31;
    case 11:
      return 30;
    case 12:
      return 31;
    default:
      return 0;
    }
  }
  [[nodiscard]] static constexpr bool is_valid(year_type y, month_type m, day_type d) {
    return y.value() >= 1 &&
        m.value() >= 1 &&
        m.value() <= 12 &&
        d.value() >= 1 &&
        d.value() <= days_in_month(y, m);
  }

  [[nodiscard]] constexpr bool operator==(const date& other) const  {
    return year_.value() == other.year_.value() &&
        month_.value() == other.month_.value() &&
        day_.value() == other.day_.value();
  }
  [[nodiscard]] constexpr bool operator!=(const date& other) const {
    return !(*this == other);
  }
  [[nodiscard]] constexpr bool operator<(const date& other) const  {
    return std::make_tuple(year_.value(), month_.value(), day_.value()) <
        std::make_tuple(other.year_.value(), other.month_.value(), other.day_.value());
  }
  [[nodiscard]] constexpr bool operator>(const date& other) const {
    return other < *this;
  }
  [[nodiscard]] constexpr bool operator<=(const date& other) const {
    return !(other < *this);
  }
  [[nodiscard]] constexpr bool operator>=(const date& other) const {
    return !(*this < other);
  }

private:
  struct current_date_parts {
    int year;
    unsigned month;
    unsigned day;
  };

  explicit date(current_date_parts parts);

  constexpr date(const year_type y, const month_type m, const day_type d)
  : year_(y), month_(m), day_(d) {
  }

  static current_date_parts now();

  year_type year_;
  month_type month_;
  day_type day_;
};

inline constexpr date::month_type january{1};
inline constexpr date::month_type jan = january;

inline constexpr date::month_type february{2};
inline constexpr date::month_type feb = february;

inline constexpr date::month_type march{3};
inline constexpr date::month_type mar = march;

inline constexpr date::month_type april{4};
inline constexpr date::month_type apr = april;

inline constexpr date::month_type may{5};

inline constexpr date::month_type june{6};
inline constexpr date::month_type jun = june;

inline constexpr date::month_type july{7};
inline constexpr date::month_type jul = july;

inline constexpr date::month_type august{8};
inline constexpr date::month_type aug = august;

inline constexpr date::month_type september{9};
inline constexpr date::month_type sep = september;

inline constexpr date::month_type october{10};
inline constexpr date::month_type oct = october;

inline constexpr date::month_type november{11};
inline constexpr date::month_type nov = november;

inline constexpr date::month_type december{12};
inline constexpr date::month_type dec = december;

constexpr date::year_type operator ""_year(const unsigned long long y) {
  return date::year_type{static_cast<int>(y)};
}
constexpr date::month_type operator ""_month(const unsigned long long m) {
  return date::month_type{static_cast<unsigned>(m)};
}
constexpr date::day_type operator ""_day(const unsigned long long d) {
  return date::day_type{static_cast<unsigned>(d)};
}
}

#endif /* MATADOR_DATE_HPP */
