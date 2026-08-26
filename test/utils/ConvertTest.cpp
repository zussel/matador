#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "matador/utils/convert.hpp"
#include "matador/utils/types.hpp"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

using namespace matador::utils;

namespace {

template<typename From, typename To>
void require_successful_conversion(const From &from, const To &expected)
{
  const auto res = to<To>(from);

  REQUIRE(res.is_ok());
  REQUIRE(*res == expected);
}

template<typename To, typename From>
void require_failed_conversion(const From &from, conversion_error expected_error = conversion_error::NotConvertable)
{
  const auto res = to<To>(from);

  REQUIRE(res.is_error());
  REQUIRE(res.err() == expected_error);
}

template<typename To, typename From>
void require_missing_data(const From &from)
{
  require_failed_conversion<To>(from, conversion_error::MissingData);
}

template<typename To, typename From>
void require_relaxed_conversion(const From &from, const To &expected)
{
  const auto res = to<To>(from, conversion_policy::Relax);

  REQUIRE(res.is_ok());
  REQUIRE(*res == expected);
}

} // namespace

TEST_CASE("Convert identical values", "[convert][identity]")
{
  require_successful_conversion<int32_t, int32_t>(42, 42);
  require_successful_conversion<std::string, std::string>("matador", "matador");

  const blob_type_t blob{1, 2, 3, 4};
  require_successful_conversion<blob_type_t, blob_type_t>(blob, blob);

  constexpr date_type_t date{2024, 8, 26};
  require_successful_conversion<date_type_t, date_type_t>(date, date);

  constexpr time_type_t time{12, 34, 56, 0};
  require_successful_conversion<time_type_t, time_type_t>(time, time);
}

TEST_CASE("Convert integral values in strict mode", "[convert][integral][strict]")
{
  require_successful_conversion<int8_t, int16_t>(-56, -56);
  require_successful_conversion<int16_t, int32_t>(-127, -127);
  require_successful_conversion<int32_t, int64_t>(-9876543, -9876543);
  require_successful_conversion<uint8_t, uint16_t>(56U, 56U);
  require_successful_conversion<uint16_t, uint32_t>(127U, 127U);
  require_successful_conversion<uint32_t, uint64_t>(9876543U, 9876543U);

  require_successful_conversion<uint8_t, int16_t>(56, 56);
  require_successful_conversion<int16_t, uint8_t>(56U, 56);
}

TEST_CASE("Reject integral overflows in strict mode", "[convert][integral][strict][overflow]")
{
  require_failed_conversion<int8_t>(int16_t{128});
  require_failed_conversion<int8_t>(int16_t{-129});
  require_failed_conversion<uint8_t>(int16_t{-1});
  require_failed_conversion<int16_t>(uint32_t{70000U});
  require_failed_conversion<uint16_t>(int32_t{-1});
  require_failed_conversion<int32_t>(uint64_t{static_cast<uint64_t>(std::numeric_limits<int32_t>::max()) + 1U});
}

TEST_CASE("Allow integral truncation in relaxed mode", "[convert][integral][relax]")
{
  require_relaxed_conversion<int8_t>(int16_t{513}, static_cast<int8_t>(513));
  require_relaxed_conversion<int8_t>(int32_t{515}, static_cast<int8_t>(515));
  require_relaxed_conversion<int8_t>(int64_t{516}, static_cast<int8_t>(516));
  require_relaxed_conversion<uint8_t>(int16_t{-1}, static_cast<uint8_t>(-1));
}

TEST_CASE("Convert floating point values", "[convert][floating_point]")
{
  const auto float_to_double = to<double>(-0.1F);
  REQUIRE(float_to_double.is_ok());
  REQUIRE(*float_to_double == Catch::Approx(-0.1));

  const auto double_to_float = to<float>(-0.44444);
  REQUIRE(double_to_float.is_ok());
  REQUIRE(*double_to_float == Catch::Approx(-0.44444F));

  const auto double_to_double = to<double>(-127.444449);
  REQUIRE(double_to_double.is_ok());
  REQUIRE(*double_to_double == Catch::Approx(-127.444449));
}

TEST_CASE("Reject invalid floating point casts in strict mode", "[convert][floating_point][strict]")
{
  require_successful_conversion<double, int32_t>(42, 42.0);
  require_successful_conversion<int32_t, double>(42.0, 42);

  require_failed_conversion<int32_t>(42.5);
  require_failed_conversion<int32_t>(std::numeric_limits<double>::infinity());
  require_failed_conversion<float>(std::numeric_limits<double>::max());
}

TEST_CASE("Allow floating point truncation in relaxed mode", "[convert][floating_point][relax]")
{
  require_relaxed_conversion<int32_t>(42.9, 42);
  require_relaxed_conversion<int32_t>(-42.9, -42);
}

TEST_CASE("Convert arithmetic values to string", "[convert][arithmetic][string]")
{
  require_successful_conversion<int8_t, std::string>(-56, "-56");
  require_successful_conversion<int16_t, std::string>(-127, "-127");
  require_successful_conversion<int32_t, std::string>(-9876543, "-9876543");
  require_successful_conversion<int64_t, std::string>(-123456790, "-123456790");

  require_successful_conversion<uint8_t, std::string>(56, "56");
  require_successful_conversion<uint16_t, std::string>(127, "127");
  require_successful_conversion<uint32_t, std::string>(9876543, "9876543");
  require_successful_conversion<uint64_t, std::string>(123456790, "123456790");

  require_successful_conversion<bool, std::string>(true, "true");
  require_successful_conversion<bool, std::string>(false, "false");

  require_successful_conversion<float, std::string>(-56.1234F, "-56.1234");
  require_successful_conversion<double, std::string>(-127.444449, "-127.444449");
}

TEST_CASE("Convert const char pointer to string", "[convert][char_pointer][string]")
{
  require_successful_conversion<const char*, std::string>("matador", "matador");

  const char *null_text = nullptr;
  const auto res = to<std::string>(null_text);

  REQUIRE(res.is_ok());
  REQUIRE(res->empty());
}

TEST_CASE("Convert strings to signed integral values", "[convert][string][integral][signed]")
{
  require_successful_conversion<std::string, int8_t>("-56", -56);
  require_successful_conversion<std::string, int16_t>("-127", -127);
  require_successful_conversion<std::string, int32_t>("-9876543", -9876543);
  require_successful_conversion<std::string, int64_t>("-123456790", -123456790);

  require_successful_conversion<const char*, int8_t>("-56", -56);
  require_successful_conversion<const char*, int16_t>("-127", -127);
  require_successful_conversion<const char*, int32_t>("-9876543", -9876543);
  require_successful_conversion<const char*, int64_t>("-123456790", -123456790);
}

TEST_CASE("Convert strings to unsigned integral values", "[convert][string][integral][unsigned]")
{
  require_successful_conversion<std::string, uint8_t>("56", 56U);
  require_successful_conversion<std::string, uint16_t>("127", 127U);
  require_successful_conversion<std::string, uint32_t>("9876543", 9876543U);
  require_successful_conversion<std::string, uint64_t>("123456790", 123456790U);

  require_successful_conversion<const char*, uint8_t>("56", 56U);
  require_successful_conversion<const char*, uint16_t>("127", 127U);
  require_successful_conversion<const char*, uint32_t>("9876543", 9876543U);
  require_successful_conversion<const char*, uint64_t>("123456790", 123456790U);
}

TEST_CASE("Reject invalid string to integral conversions", "[convert][string][integral][invalid]")
{
  require_missing_data<int32_t>(std::string{});
  require_missing_data<int32_t>("");

  const char *null_text = nullptr;
  require_missing_data<int32_t>(null_text);

  require_failed_conversion<int32_t>(std::string{"abc"});
  require_failed_conversion<int32_t>(std::string{"123abc"});
  require_failed_conversion<int32_t>(std::string{"12.3"});
  require_failed_conversion<int32_t>(std::string{" "});
  require_failed_conversion<uint32_t>(std::string{"-1"});
  require_failed_conversion<int8_t>(std::string{"128"});
  require_failed_conversion<int8_t>(std::string{"-129"});
  require_failed_conversion<uint8_t>(std::string{"256"});
}

TEST_CASE("Convert strings to floating point values", "[convert][string][floating_point]")
{
  const auto float_res = to<float>(std::string{"-56.1234"});
  REQUIRE(float_res.is_ok());
  REQUIRE(*float_res == Catch::Approx(-56.1234F));

  const auto double_res = to<double>(std::string{"-127.444449"});
  REQUIRE(double_res.is_ok());
  REQUIRE(*double_res == Catch::Approx(-127.444449));

  const auto char_float_res = to<float>("-56.1234");
  REQUIRE(char_float_res.is_ok());
  REQUIRE(*char_float_res == Catch::Approx(-56.1234F));

  const auto char_double_res = to<double>("-127.444449");
  REQUIRE(char_double_res.is_ok());
  REQUIRE(*char_double_res == Catch::Approx(-127.444449));
}

TEST_CASE("Reject invalid string to floating point conversions", "[convert][string][floating_point][invalid]")
{
  require_missing_data<double>(std::string{});
  require_missing_data<double>("");

  const char *null_text = nullptr;
  require_missing_data<double>(null_text);

  require_failed_conversion<double>(std::string{"abc"});
  require_failed_conversion<double>(std::string{"12.3abc"});
  require_failed_conversion<double>(std::string{" "});
}

TEST_CASE("Convert strings to bool", "[convert][string][bool]")
{
  require_successful_conversion<std::string, bool>("true", true);
  require_successful_conversion<std::string, bool>("t", true);
  require_successful_conversion<std::string, bool>("on", true);
  require_successful_conversion<std::string, bool>("1", true);

  require_successful_conversion<std::string, bool>("false", false);
  require_successful_conversion<std::string, bool>("f", false);
  require_successful_conversion<std::string, bool>("off", false);
  require_successful_conversion<std::string, bool>("0", false);

  require_successful_conversion<std::string, bool>("TRUE", true);
  require_successful_conversion<std::string, bool>("False", false);
  require_successful_conversion<const char*, bool>("ON", true);
  require_successful_conversion<const char*, bool>("Off", false);
}

TEST_CASE("Reject invalid string to bool conversions", "[convert][string][bool][invalid]")
{
  require_missing_data<bool>(std::string{});
  require_missing_data<bool>("");

  const char *null_text = nullptr;
  require_missing_data<bool>(null_text);

  require_failed_conversion<bool>(std::string{"yes"});
  require_failed_conversion<bool>(std::string{"no"});
  require_failed_conversion<bool>(std::string{"2"});
  require_failed_conversion<bool>(std::string{" true "});
}

TEST_CASE("Convert arithmetic values to bool", "[convert][arithmetic][bool]")
{
  require_successful_conversion<int32_t, bool>(0, false);
  require_successful_conversion<int32_t, bool>(1, true);
  require_successful_conversion<int32_t, bool>(42, true);
  require_successful_conversion<double, bool>(0.0, false);
  require_successful_conversion<double, bool>(0.5, true);
}

TEST_CASE("Convert bool to arithmetic values", "[convert][bool][arithmetic]")
{
  require_successful_conversion<bool, int32_t>(true, 1);
  require_successful_conversion<bool, int32_t>(false, 0);
  require_successful_conversion<bool, double>(true, 1.0);
  require_successful_conversion<bool, double>(false, 0.0);
}

TEST_CASE("Convert strings to date and time values", "[convert][string][date_time]")
{
  require_successful_conversion<std::string, date_type_t>(
    "2024-08-26",
    date_type_t{2024, 8, 26});

  require_successful_conversion<const char*, date_type_t>(
    "2024-08-26",
    date_type_t{2024, 8, 26});

  require_successful_conversion<std::string, time_type_t>(
    "12:34:56",
    time_type_t{12, 34, 56, 0});

  require_successful_conversion<const char*, time_type_t>(
    "12:34:56",
    time_type_t{12, 34, 56, 0});
}

TEST_CASE("Parse date without locale dependent get_time", "[convert][date][parse]")
{
  const auto res = to<date_type_t>(std::string{"2024-08-26"});

  REQUIRE(res.is_ok());
  REQUIRE(*res == date_type_t{2024, 8, 26});
}

TEST_CASE("Reject invalid calendar dates", "[convert][date][parse][invalid]")
{
  require_failed_conversion<date_type_t>(std::string{"2024-00-26"});
  require_failed_conversion<date_type_t>(std::string{"2024-13-26"});
  require_failed_conversion<date_type_t>(std::string{"2024-08-00"});
  require_failed_conversion<date_type_t>(std::string{"2024-08-32"});
  require_failed_conversion<date_type_t>(std::string{"2024-02-30"});
  require_failed_conversion<date_type_t>(std::string{"2023-02-29"});
  require_failed_conversion<date_type_t>(std::string{"2024-8-26"});
  require_failed_conversion<date_type_t>(std::string{"2024-08-26abc"});
}

TEST_CASE("Reject invalid clock times", "[convert][time][parse][invalid]")
{
  require_failed_conversion<time_type_t>(std::string{"24:00:00"});
  require_failed_conversion<time_type_t>(std::string{"12:60:00"});
  require_failed_conversion<time_type_t>(std::string{"12:00:60"});
  require_failed_conversion<time_type_t>(std::string{"1:02:03"});
  require_failed_conversion<time_type_t>(std::string{"12:34:56abc"});
}

TEST_CASE("Parse timestamp without locale dependent get_time", "[convert][timestamp][parse]")
{
  const auto res = to<timestamp_type_t>(std::string{"2024-08-26 12:34:56"});

  REQUIRE(res.is_ok());

  const auto date = to<date_type_t>(*res);
  REQUIRE(date.is_ok());
  REQUIRE(*date == date_type_t{2024, 8, 26});

  const auto time = to<time_type_t>(*res);
  REQUIRE(time.is_ok());
  REQUIRE(*time == time_type_t{12, 34, 56, 0});
}

TEST_CASE("Reject invalid timestamp values", "[convert][timestamp][parse][invalid]")
{
  require_failed_conversion<timestamp_type_t>(std::string{"2024-08-26T12:34:56"});
  require_failed_conversion<timestamp_type_t>(std::string{"2024-08-26 24:00:00"});
  require_failed_conversion<timestamp_type_t>(std::string{"2024-02-30 12:34:56"});
  require_failed_conversion<timestamp_type_t>(std::string{"2024-08-26 12:34:56abc"});
}

TEST_CASE("Reject invalid date and time strings", "[convert][string][date_time][invalid]")
{
  require_missing_data<date_type_t>(std::string{});
  require_missing_data<time_type_t>(std::string{});

  const char *null_text = nullptr;
  require_missing_data<date_type_t>(null_text);
  require_missing_data<time_type_t>(null_text);

  require_failed_conversion<date_type_t>(std::string{"2024-08-26abc"});
  require_failed_conversion<date_type_t>(std::string{"not-a-date"});
  require_failed_conversion<time_type_t>(std::string{"12:34:56abc"});
  require_failed_conversion<time_type_t>(std::string{"not-a-time"});
}

TEST_CASE("Convert date and time values to string", "[convert][date_time][string]")
{
  require_successful_conversion<date_type_t, std::string>(
    date_type_t{2024, 8, 26},
    "2024-08-26");

  require_successful_conversion<date_type_t, std::string>(
    date_type_t{7, 1, 2},
    "0007-01-02");

  require_successful_conversion<time_type_t, std::string>(
    time_type_t{12, 34, 56, 0},
    "12:34:56");

  require_successful_conversion<time_type_t, std::string>(
    time_type_t{1, 2, 3, 0},
    "01:02:03");
}

TEST_CASE("Convert strings to timestamp", "[convert][string][timestamp]")
{
  const auto res = to<timestamp_type_t>(std::string{"2024-08-26 12:34:56"});

  REQUIRE(res.is_ok());

  const auto date_res = to<date_type_t>(*res);
  REQUIRE(date_res.is_ok());
  REQUIRE(*date_res == date_type_t{2024, 8, 26});

  const auto time_res = to<time_type_t>(*res);
  REQUIRE(time_res.is_ok());
  REQUIRE(time_res->hour == 12);
  REQUIRE(time_res->minute == 34);
  REQUIRE(time_res->second == 56);
}

TEST_CASE("Reject invalid timestamp strings", "[convert][string][timestamp][invalid]")
{
  require_missing_data<timestamp_type_t>(std::string{});

  const char *null_text = nullptr;
  require_missing_data<timestamp_type_t>(null_text);

  require_failed_conversion<timestamp_type_t>(std::string{"2024-08-26"});
  require_failed_conversion<timestamp_type_t>(std::string{"2024-08-26 12:34:56abc"});
  require_failed_conversion<timestamp_type_t>(std::string{"not-a-timestamp"});
}

TEST_CASE("Convert timestamp to date time and string", "[convert][timestamp]")
{
  const auto timestamp_res = to<timestamp_type_t>(std::string{"2024-08-26 12:34:56"});
  REQUIRE(timestamp_res.is_ok());

  const auto date_res = to<date_type_t>(*timestamp_res);
  REQUIRE(date_res.is_ok());
  REQUIRE(*date_res == date_type_t{2024, 8, 26});

  const auto time_res = to<time_type_t>(*timestamp_res);
  REQUIRE(time_res.is_ok());
  REQUIRE(*time_res == time_type_t{12, 34, 56, 0});

  const auto string_res = to<std::string>(*timestamp_res);
  REQUIRE(string_res.is_ok());
  REQUIRE(*string_res == "2024-08-26 12:34:56");
}

TEST_CASE("Convert values to blob", "[convert][blob]")
{
  const auto int_res = to<blob_type_t>(int32_t{0x01020304});
  REQUIRE(int_res.is_ok());
  REQUIRE(int_res->size() == sizeof(int32_t));

  int32_t restored_int{};
  std::memcpy(&restored_int, int_res->data(), sizeof(restored_int));
  REQUIRE(restored_int == 0x01020304);

  const auto bool_res = to<blob_type_t>(true);
  REQUIRE(bool_res.is_ok());
  REQUIRE(bool_res->size() == sizeof(bool));

  const auto string_res = to<blob_type_t>(std::string{"abc"});
  REQUIRE(string_res.is_ok());
  REQUIRE(*string_res == blob_type_t{'a', 'b', 'c'});

  const auto char_res = to<blob_type_t>("abc");
  REQUIRE(char_res.is_ok());
  REQUIRE(*char_res == blob_type_t{'a', 'b', 'c'});
}

TEST_CASE("Convert blob to trivially copyable values", "[convert][blob][trivial]")
{
  const int32_t original = 0x01020304;
  const auto blob_res = to<blob_type_t>(original);

  REQUIRE(blob_res.is_ok());

  const auto restored_res = to<int32_t>(*blob_res);

  REQUIRE(restored_res.is_ok());
  REQUIRE(*restored_res == original);

  const auto bool_blob_res = to<blob_type_t>(true);

  REQUIRE(bool_blob_res.is_ok());

  const auto restored_bool_res = to<bool>(*bool_blob_res);

  REQUIRE(restored_bool_res.is_ok());
  REQUIRE(*restored_bool_res);
}

TEST_CASE("Reject empty blob to trivial conversion", "[convert][blob][invalid]")
{
  const blob_type_t empty_blob;

  require_missing_data<int32_t>(empty_blob);
  require_missing_data<bool>(empty_blob);
}

TEST_CASE("Reject unsupported conversions", "[convert][unsupported]")
{
  require_failed_conversion<date_type_t>(true);
  require_failed_conversion<time_type_t>(false);
  require_failed_conversion<timestamp_type_t>(true);

  require_failed_conversion<bool>(date_type_t{2024, 8, 26});
  require_failed_conversion<double>(date_type_t{2024, 8, 26});
  require_failed_conversion<int32_t>(time_type_t{12, 34, 56, 0});

  require_failed_conversion<std::string>(blob_type_t{1, 2, 3});
}