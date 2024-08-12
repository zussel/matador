#ifndef MATADOR_CONVERT_HPP
#define MATADOR_CONVERT_HPP

#include "matador/utils/types.hpp"
#include "matador/utils/result.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/utils/placeholder.hpp"

#include <array>
#include <charconv>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

/*
 * Conversion matrix
 *       from> | int8 | int16 | int32 | int64 | uint8 | uint16 | uint32 | uint64 | bool | float | double | string | date | time | blob | placeholder | null
 *          to |      |       |       |       |       |        |        |        |      |       |        |        |      |      |      |             |
 * ------------+------+-------+-------+-------+-------+--------+--------+--------+------+-------+--------+--------+------+------+------+-------------+-----
 *        int8 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *       int16 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *       int32 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *       int64 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *       uint8 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *      uint16 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *      uint32 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *      uint64 |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  ok  |  ok  |  try |     N/A     | N/A
 *        bool |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  N/A |  N/A |  try |     N/A     | N/A
 *       float |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  N/A |  N/A |  try |     N/A     | N/A
 *      double |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |  try   |  N/A |  N/A |  try |     N/A     | N/A
 *      string |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |   ok   |  ok  |  ok  |  N/A |     N/A     | N/A
 *        date |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  N/A |  N/A  |  N/A   |  try   |  ok  |  ok  |  N/A |     N/A     | N/A
 *        time |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  N/A |  N/A  |  N/A   |  try   |  ok  |  ok  |  N/A |     N/A     | N/A
 *        blob |  ok  |  ok   |  ok   |  ok   |   ok  |   ok   |   ok   |   ok   |  ok  |  ok   |  ok    |   ok   |  ok  |  ok  |  ok  |     N/A     | N/A
 * placeholder | N/A  |  N/A  |  N/A  |  N/A  |   N/A |   N/A  |   N/A  |   N/A  |  N/A |  N/A  |  N/A   |   N/A  |  N/A |  N/A |  N/A |     ok      | N/A
 *        null | N/A  |  N/A  |  N/A  |  N/A  |   N/A |   N/A  |   N/A  |   N/A  |  N/A |  N/A  |  N/A   |   N/A  |  N/A |  N/A |  N/A |     N/A     | ok
 *
 * from integral to date/time works, value is interpreted as std::chrono::timepoint
 * from integral to blob works, bytes of integral data will converted to blob
 * from boolean to blob works, byte of bool data will converted to blob
 * from floating point to blob works, bytes of floating point data will converted to blob
 * from date to blob works, bytes of date will converted to blob
 * from time to blob works, bytes of time will converted to blob
 * from string to blob works, bytes of string data will converted to blob
 * from floating point to integral works, fractions are truncated
 * from date/time to integral works, value will be converted from std::chrono::timepoint
 * from blob to integral works, value will be converted missing is filled with zero, rest is omitted
 * from blob to floating point works, value will be converted missing is filled with zero, rest is omitted
 * from blob to boolean works, value will be converted missing is filled with zero, rest is omitted
 */

namespace matador::utils {

enum class conversion_error {
  Ok,
  NotConvertable,
  MissingData
};

enum class conversion_policy {
  Strict,
  Relax
};

template < typename DestType, typename SourceType >
utils::result<DestType, conversion_error> to(const SourceType &from, conversion_policy policy = conversion_policy::Strict)
{
  return utils::error(conversion_error::NotConvertable);
}

/*
 * Integral, Floating point & bool conversion
 */
template < typename DestType, typename SourceType >
void convert(DestType &dest, SourceType source, typename std::enable_if<std::is_arithmetic<DestType>::value && std::is_arithmetic<SourceType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source);
}

/*
 * Placeholder & nullptr type conversion
 */
void convert(placeholder &dest, const placeholder &source);
void convert(nullptr_t &dest, const nullptr_t &source);

template < typename SourceType >
void convert(placeholder &/*dest*/, const SourceType &/*source*/)
{
  throw std::logic_error("couldn't convert value to placeholder");
}

template < typename SourceType >
void convert(nullptr_t &/*dest*/, const SourceType &/*source*/)
{
  throw std::logic_error("couldn't convert value to nullptr type");
}

template < typename DestType >
void convert(DestType &/*dest*/, const placeholder &/*source*/)
{
  throw std::logic_error("couldn't convert value from placeholder");
}

template < typename DestType >
void convert(DestType &/*dest*/, const nullptr_t &/*source*/)
{
  throw std::logic_error("couldn't convert value from nullptr type");
}

/*
 * Integral to string, date, time & blob conversion
 */
template < typename SourceType >
void convert(std::string &dest, SourceType source, typename std::enable_if<std::is_integral<SourceType>::value && !std::is_same<bool, SourceType>::value>::type* = nullptr)
{
  std::array<char, 128> buffer{};
  auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), source, 10);
  if (ec == std::errc{}) {
    dest.assign(buffer.data(), ptr);
  } else {
    throw std::logic_error("couldn't convert value to std::string");
  }
}

template < typename SourceType >
void convert(date &dest, SourceType source, typename std::enable_if<std::is_integral<SourceType>::value && !std::is_same<bool, SourceType>::value>::type* = nullptr)
{
  dest.set(static_cast<int>(source));
}

template < typename SourceType >
void convert(time &dest, SourceType source, typename std::enable_if<std::is_integral<SourceType>::value && !std::is_same<bool, SourceType>::value>::type* = nullptr)
{
  dest.set(static_cast<time_t>(source), 0);
}

template < typename SourceType >
void convert(utils::blob &dest, SourceType source, typename std::enable_if<std::is_integral<SourceType>::value && !std::is_same<bool, SourceType>::value>::type* = nullptr)
{
  dest.resize(sizeof(source));
  std::memcpy(dest.data(), &source, sizeof(source));
}

void convert(std::string &dest, bool source);

template < typename SourceType >
void convert(std::string &dest, SourceType source, typename std::enable_if<std::is_floating_point<SourceType>::value>::type* = nullptr)
{
  std::array<char, 128> buffer{};
  auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), source);
  if (ec == std::errc{}) {
    dest.assign(buffer.data(), ptr);
  } else {
    throw std::logic_error("couldn't convert value to std::string");
  }
}

void convert(std::string &dest, const char* source);
void convert(matador::date &dest, const char* source);
void convert(matador::time &dest, const char* source);

unsigned long long to_unsigned_long_long(const char *source);

/*
 * String to unsigned integral
 */
template < typename DestType >
void convert(DestType &dest, const std::string &source, typename std::enable_if<std::is_integral<DestType>::value && std::is_unsigned<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_unsigned_long_long(source.c_str()));
}

template < typename DestType >
void convert(DestType &dest, const char *source, typename std::enable_if<std::is_integral<DestType>::value && std::is_unsigned<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_unsigned_long_long(source));
}

long long to_long_long(const char *source);

/*
 * String to signed integral
 */
template < typename DestType >
void convert(DestType &dest, const std::string &source, typename std::enable_if<std::is_integral<DestType>::value && std::is_signed<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_long_long(source.c_str()));
}

template < typename DestType >
void convert(DestType &dest, const char *source, typename std::enable_if<std::is_integral<DestType>::value && std::is_signed<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_long_long(source));
}

long double to_double(const char *source);

template < typename DestType >
void convert(DestType &dest, const std::string &source, typename std::enable_if<std::is_floating_point<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_double(source.c_str()));
}

template < typename DestType >
void convert(DestType &dest, const char *source, typename std::enable_if<std::is_floating_point<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(to_double(source));
}

void convert(std::string &dest,const std::string &source);

/*
 * Date & time to integral
 */
template < typename DestType >
void convert(DestType &dest, const date &source, typename std::enable_if<std::is_integral<DestType>::value && !std::is_same<DestType, bool>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source.julian_date());
}

template < typename DestType >
void convert(DestType &dest, const time &source, typename std::enable_if<std::is_integral<DestType>::value && !std::is_same<DestType, bool>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source.get_time_info().seconds_since_epoch);
}

/*
 * Date & time to floating point
 */
template < typename DestType >
void convert(DestType &/*dest*/, const date &/*source*/, typename std::enable_if<std::is_floating_point<DestType>::value>::type* = nullptr)
{
  throw std::logic_error("couldn't convert value to matador::date");
}

template < typename DestType >
void convert(DestType &/*dest*/, const time &/*source*/, typename std::enable_if<std::is_floating_point<DestType>::value>::type* = nullptr)
{
  throw std::logic_error("couldn't convert value to matador::time");
}

/*
 * Date & time to bool
 */
void convert(bool &/*dest*/, const date &/*source*/);
void convert(bool &/*dest*/, const time &/*source*/);

/*
 * Date & time to string
 */
void convert(std::string &dest, const date &source);
void convert(std::string &dest, const time &source);

/*
 * Date & time to blob
 */
void convert(utils::blob &dest, const date &source);
void convert(utils::blob &dest, const time &source);

/*
 * floating point to blob
 */
template < typename SourceType >
void convert(utils::blob &/*dest*/, const SourceType &/*source*/, typename std::enable_if<std::is_floating_point<SourceType>::value>::type* = nullptr)
{
  throw std::logic_error("couldn't convert floating point value to blob");
}

/*
 * bool, const char* & string to blob
 */
void convert(utils::blob &/*dest*/, bool source);
void convert(utils::blob &/*dest*/, const char *source);
void convert(utils::blob &/*dest*/, const std::string &source);

/*
 * blob, const char* & string to bool
 */
void convert(bool &dest, const std::string &source);
void convert(bool &dest, const char *source);
void convert(bool &dest, const utils::blob &source);

/*
 * Bool to date or time
 */
void convert(matador::date &dest, bool source);
void convert(matador::time &dest, bool source);

/*
 * string to date or time
 */
void convert(matador::date &dest, const std::string &source);
void convert(matador::time &dest, const std::string &source);

/*
 * date/time to date or time
 */
void convert(matador::date &dest, const matador::date &source);
void convert(matador::time &dest, const matador::date &source);
void convert(matador::date &dest, const matador::time &source);
void convert(matador::time &dest, const matador::time &source);

template < typename DestType >
void convert(DestType &dest, bool source, typename std::enable_if<std::is_floating_point<DestType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source);
}

template < typename DestType >
void convert(DestType &/*dest*/, const utils::blob &/*data*/)
{
  throw std::logic_error("couldn't convert matador::utils::blob into destination type");
}

void convert(utils::blob &dest, const utils::blob &data);

}

#endif //MATADOR_CONVERT_HPP
