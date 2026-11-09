#ifndef MATADOR_TIMESTAMP_HPP
#define MATADOR_TIMESTAMP_HPP

#include "matador/utils/date.hpp"

#include <optional>
#include <tuple>

namespace matador {
class timestamp {
public:
  using year_type = date::year_type;
  using month_type = date::month_type;
  using day_type = date::day_type;

  class hour_type {
  public:
    explicit constexpr hour_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  class minute_type {
  public:
    explicit constexpr minute_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  class second_type {
  public:
    explicit constexpr second_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  class millisecond_type {
  public:
    explicit constexpr millisecond_type(const unsigned v) : value_(v) {}

    [[nodiscard]] constexpr unsigned value() const { return value_; }

  private:
    unsigned value_;
  };

  timestamp();

  constexpr timestamp(
      const year_type y,
      const month_type mo,
      const day_type d,
      const hour_type h,
      const minute_type mi,
      const second_type s,
      const millisecond_type ms
  )
      : year_(y),
        month_(mo),
        day_(d),
        hour_(h),
        minute_(mi),
        second_(s),
        millisecond_(ms)
  {
  }

  [[nodiscard]] constexpr year_type year() const { return year_; }
  [[nodiscard]] constexpr month_type month() const { return month_; }
  [[nodiscard]] constexpr day_type day() const { return day_; }
  [[nodiscard]] constexpr hour_type hour() const { return hour_; }
  [[nodiscard]] constexpr minute_type minute() const { return minute_; }
  [[nodiscard]] constexpr second_type second() const { return second_; }
  [[nodiscard]] constexpr millisecond_type millisecond() const { return millisecond_; }

  [[nodiscard]] static constexpr bool is_valid(
      const year_type y,
      const month_type mo,
      const day_type d,
      const hour_type h,
      const minute_type mi,
      const second_type s,
      const millisecond_type ms
  ) {
    return date::is_valid(y, mo, d) &&
           h.value() <= 23 &&
           mi.value() <= 59 &&
           s.value() <= 59 &&
           ms.value() <= 999;
  }

  [[nodiscard]] constexpr bool is_valid() const {
    return is_valid(year_, month_, day_, hour_, minute_, second_, millisecond_);
  }

  [[nodiscard]] static constexpr std::optional<timestamp> make(
      const year_type y,
      const month_type mo,
      const day_type d,
      const hour_type h,
      const minute_type mi,
      const second_type s,
      const millisecond_type ms
  ) {
    if (!is_valid(y, mo, d, h, mi, s, ms)) {
      return std::nullopt;
    }

    return timestamp{y, mo, d, h, mi, s, ms};
  }

  [[nodiscard]] constexpr bool operator==(const timestamp& other) const {
    return year_.value() == other.year_.value() &&
           month_.value() == other.month_.value() &&
           day_.value() == other.day_.value() &&
           hour_.value() == other.hour_.value() &&
           minute_.value() == other.minute_.value() &&
           second_.value() == other.second_.value() &&
           millisecond_.value() == other.millisecond_.value();
  }

  [[nodiscard]] constexpr bool operator!=(const timestamp& other) const {
    return !(*this == other);
  }

  [[nodiscard]] constexpr bool operator<(const timestamp& other) const {
    return std::make_tuple(
               year_.value(),
               month_.value(),
               day_.value(),
               hour_.value(),
               minute_.value(),
               second_.value(),
               millisecond_.value()
           ) <
           std::make_tuple(
               other.year_.value(),
               other.month_.value(),
               other.day_.value(),
               other.hour_.value(),
               other.minute_.value(),
               other.second_.value(),
               other.millisecond_.value()
           );
  }

  [[nodiscard]] constexpr bool operator>(const timestamp& other) const {
    return other < *this;
  }

  [[nodiscard]] constexpr bool operator<=(const timestamp& other) const {
    return !(other < *this);
  }

  [[nodiscard]] constexpr bool operator>=(const timestamp& other) const {
    return !(*this < other);
  }

private:
  struct current_timestamp_parts {
    int year;
    unsigned month;
    unsigned day;
    unsigned hour;
    unsigned minute;
    unsigned second;
    unsigned millisecond;
  };

  explicit timestamp(const current_timestamp_parts& parts)
      : year_(year_type{parts.year}),
        month_(month_type{parts.month}),
        day_(day_type{parts.day}),
        hour_(hour_type{parts.hour}),
        minute_(minute_type{parts.minute}),
        second_(second_type{parts.second}),
        millisecond_(millisecond_type{parts.millisecond})
  {
  }

  static current_timestamp_parts now();

  year_type year_;
  month_type month_;
  day_type day_;
  hour_type hour_;
  minute_type minute_;
  second_type second_;
  millisecond_type millisecond_;
};

constexpr timestamp::hour_type operator ""_hour(const unsigned long long h) {
  return timestamp::hour_type{static_cast<unsigned>(h)};
}

constexpr timestamp::minute_type operator ""_minute(const unsigned long long m) {
  return timestamp::minute_type{static_cast<unsigned>(m)};
}

constexpr timestamp::second_type operator ""_second(const unsigned long long s) {
  return timestamp::second_type{static_cast<unsigned>(s)};
}

constexpr timestamp::millisecond_type operator ""_millisecond(const unsigned long long ms) {
  return timestamp::millisecond_type{static_cast<unsigned>(ms)};
}
}

#endif /* MATADOR_TIMESTAMP_HPP */