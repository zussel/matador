#include <catch2/catch_test_macros.hpp>

#include "matador/utils/date.hpp"

using namespace matador;

TEST_CASE("Date: make creates valid dates", "[date][make]") {
  const auto d = date::make(2024_year, feb, 29_day);

  REQUIRE(d.has_value());
  REQUIRE(d->year().value() == 2024);
  REQUIRE(d->month().value() == 2);
  REQUIRE(d->day().value() == 29);
}

TEST_CASE("Date: make rejects invalid years", "[date][make][validation]") {
  REQUIRE_FALSE(date::make(0_year, jan, 1_day).has_value());
  REQUIRE_FALSE(date::make(static_cast<date::year_type>(date::year_type{-1}), jan, 1_day).has_value());
}

TEST_CASE("Date: make rejects invalid months", "[date][make][validation]") {
  REQUIRE_FALSE(date::make(2024_year, 0_month, 1_day).has_value());
  REQUIRE_FALSE(date::make(2024_year, 13_month, 1_day).has_value());
}

TEST_CASE("Date: make rejects invalid days", "[date][make][validation]") {
  REQUIRE_FALSE(date::make(2024_year, jan, 0_day).has_value());
  REQUIRE_FALSE(date::make(2024_year, jan, 32_day).has_value());
  REQUIRE_FALSE(date::make(2024_year, apr, 31_day).has_value());
  REQUIRE_FALSE(date::make(2023_year, feb, 29_day).has_value());
}

TEST_CASE("Date: leap year calculation follows Gregorian calendar rules", "[date][leap-year]") {
  REQUIRE(date::is_leap_year(2024_year));
  REQUIRE(date::is_leap_year(2000_year));

  REQUIRE_FALSE(date::is_leap_year(2023_year));
  REQUIRE_FALSE(date::is_leap_year(1900_year));
  REQUIRE_FALSE(date::is_leap_year(2100_year));
}

TEST_CASE("Date: days_in_month returns correct values for common year", "[date][days-in-month]") {
  REQUIRE(date::days_in_month(2023_year, jan) == 31);
  REQUIRE(date::days_in_month(2023_year, feb) == 28);
  REQUIRE(date::days_in_month(2023_year, mar) == 31);
  REQUIRE(date::days_in_month(2023_year, apr) == 30);
  REQUIRE(date::days_in_month(2023_year, may) == 31);
  REQUIRE(date::days_in_month(2023_year, jun) == 30);
  REQUIRE(date::days_in_month(2023_year, jul) == 31);
  REQUIRE(date::days_in_month(2023_year, aug) == 31);
  REQUIRE(date::days_in_month(2023_year, sep) == 30);
  REQUIRE(date::days_in_month(2023_year, oct) == 31);
  REQUIRE(date::days_in_month(2023_year, nov) == 30);
  REQUIRE(date::days_in_month(2023_year, dec) == 31);
}

TEST_CASE("Date: days_in_month returns 29 days for February in leap year", "[date][days-in-month][leap-year]") {
  REQUIRE(date::days_in_month(2024_year, feb) == 29);
}

TEST_CASE("Date: days_in_month returns zero for invalid months", "[date][days-in-month][validation]") {
  REQUIRE(date::days_in_month(2024_year, 0_month) == 0);
  REQUIRE(date::days_in_month(2024_year, 13_month) == 0);
}

TEST_CASE("Date: is_valid accepts valid boundary dates", "[date][validation]") {
  REQUIRE(date::is_valid(1_year, jan, 1_day));
  REQUIRE(date::is_valid(2024_year, feb, 29_day));
  REQUIRE(date::is_valid(2023_year, dec, 31_day));
}

TEST_CASE("Date: is_valid rejects invalid boundary dates", "[date][validation]") {
  REQUIRE_FALSE(date::is_valid(0_year, jan, 1_day));
  REQUIRE_FALSE(date::is_valid(2024_year, 0_month, 1_day));
  REQUIRE_FALSE(date::is_valid(2024_year, 13_month, 1_day));
  REQUIRE_FALSE(date::is_valid(2024_year, jan, 0_day));
  REQUIRE_FALSE(date::is_valid(2024_year, jan, 32_day));
  REQUIRE_FALSE(date::is_valid(2024_year, apr, 31_day));
  REQUIRE_FALSE(date::is_valid(2023_year, feb, 29_day));
}

TEST_CASE("Date: equality and inequality compare year month and day", "[date][comparison]") {
  const auto lhs = date::make(2024_year, feb, 29_day);
  const auto same = date::make(2024_year, feb, 29_day);
  const auto different_day = date::make(2024_year, feb, 28_day);
  const auto different_month = date::make(2024_year, mar, 29_day);
  const auto different_year = date::make(2023_year, feb, 28_day);

  REQUIRE(lhs.has_value());
  REQUIRE(same.has_value());
  REQUIRE(different_day.has_value());
  REQUIRE(different_month.has_value());
  REQUIRE(different_year.has_value());

  REQUIRE(*lhs == *same);
  REQUIRE_FALSE(*lhs != *same);

  REQUIRE(*lhs != *different_day);
  REQUIRE(*lhs != *different_month);
  REQUIRE(*lhs != *different_year);
}

TEST_CASE("Date: less-than compares chronologically", "[date][comparison]") {
  const auto earlier_year = date::make(2023_year, dec, 31_day);
  const auto later_year = date::make(2024_year, jan, 1_day);

  const auto earlier_month = date::make(2024_year, jan, 31_day);
  const auto later_month = date::make(2024_year, feb, 1_day);

  const auto earlier_day = date::make(2024_year, feb, 28_day);
  const auto later_day = date::make(2024_year, feb, 29_day);

  REQUIRE(earlier_year.has_value());
  REQUIRE(later_year.has_value());
  REQUIRE(earlier_month.has_value());
  REQUIRE(later_month.has_value());
  REQUIRE(earlier_day.has_value());
  REQUIRE(later_day.has_value());

  REQUIRE(*earlier_year < *later_year);
  REQUIRE(*earlier_month < *later_month);
  REQUIRE(*earlier_day < *later_day);

  REQUIRE_FALSE(*later_year < *earlier_year);
  REQUIRE_FALSE(*later_month < *earlier_month);
  REQUIRE_FALSE(*later_day < *earlier_day);
}

TEST_CASE("Date: relational operators are consistent", "[date][comparison]") {
  const auto earlier = date::make(2024_year, jan, 1_day);
  const auto same = date::make(2024_year, jan, 1_day);
  const auto later = date::make(2024_year, jan, 2_day);

  REQUIRE(earlier.has_value());
  REQUIRE(same.has_value());
  REQUIRE(later.has_value());

  REQUIRE(*later > *earlier);
  REQUIRE(*later >= *earlier);
  REQUIRE(*later >= *same);

  REQUIRE(*earlier < *later);
  REQUIRE(*earlier <= *later);
  REQUIRE(*earlier <= *same);

  REQUIRE_FALSE(*earlier > *later);
  REQUIRE_FALSE(*later < *earlier);
  REQUIRE_FALSE(*earlier > *same);
  REQUIRE_FALSE(*earlier < *same);
}

TEST_CASE("Date: month constants and aliases have expected values", "[date][month-constants]") {
  REQUIRE(january.value() == 1);
  REQUIRE(jan.value() == 1);

  REQUIRE(february.value() == 2);
  REQUIRE(feb.value() == 2);

  REQUIRE(march.value() == 3);
  REQUIRE(mar.value() == 3);

  REQUIRE(april.value() == 4);
  REQUIRE(apr.value() == 4);

  REQUIRE(may.value() == 5);

  REQUIRE(june.value() == 6);
  REQUIRE(jun.value() == 6);

  REQUIRE(july.value() == 7);
  REQUIRE(jul.value() == 7);

  REQUIRE(august.value() == 8);
  REQUIRE(aug.value() == 8);

  REQUIRE(september.value() == 9);
  REQUIRE(sep.value() == 9);

  REQUIRE(october.value() == 10);
  REQUIRE(oct.value() == 10);

  REQUIRE(november.value() == 11);
  REQUIRE(nov.value() == 11);

  REQUIRE(december.value() == 12);
  REQUIRE(dec.value() == 12);
}

TEST_CASE("Date: user-defined literals create expected strongly typed values", "[date][literals]") {
  const auto year = 2024_year;
  const auto month = 12_month;
  const auto day = 31_day;

  REQUIRE(year.value() == 2024);
  REQUIRE(month.value() == 12);
  REQUIRE(day.value() == 31);
}

TEST_CASE("Date: default constructor creates a valid current local date", "[date][default-constructor]") {
  const date today;

  REQUIRE(date::is_valid(today.year(), today.month(), today.day()));
  REQUIRE(today.year().value() >= 1);
  REQUIRE(today.month().value() >= 1);
  REQUIRE(today.month().value() <= 12);
  REQUIRE(today.day().value() >= 1);
  REQUIRE(today.day().value() <= date::days_in_month(today.year(), today.month()));
}