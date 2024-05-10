#ifndef MATADOR_CONVERT_HPP
#define MATADOR_CONVERT_HPP

#include "matador/utils/types.hpp"

#include <array>
#include <charconv>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace matador {
class date;
class time;
}

namespace matador::utils {

template < typename DestType, typename SourceType >
void convert(DestType &dest, SourceType source, typename std::enable_if<std::is_same<DestType, SourceType>::value>::type* = nullptr)
{
  dest = source;
}

template < typename DestType, typename SourceType >
void convert(DestType &dest, SourceType source, typename std::enable_if<std::is_integral<DestType>::value && std::is_arithmetic<SourceType>::value && !std::is_same<DestType, SourceType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source);
}

template < typename DestType, typename SourceType >
void convert(DestType &dest, SourceType source, typename std::enable_if<std::is_floating_point<DestType>::value && std::is_arithmetic<SourceType>::value && !std::is_same<DestType, SourceType>::value>::type* = nullptr)
{
  dest = static_cast<DestType>(source);
}

void convert(std::string &dest, bool source);

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

template < typename SourceType >
void convert(utils::blob &/*dest*/, SourceType /*source*/, typename std::enable_if<!std::is_same<utils::blob, SourceType>::value>::type* = nullptr)
{
  throw std::logic_error("couldn't convert value to matador::utils::blob");
}

void convert(std::string &dest, const char* source);

unsigned long long to_unsigned_long_long(const char *source);

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

void convert(std::string &dest, const date &source);

template < typename SourceType >
void convert(date &/*dest*/, SourceType &/*source*/)
{
  throw std::logic_error("couldn't convert value into date");
}

template < typename DestType >
void convert(DestType &/*dest*/, const time &/*data*/)
{
  throw std::logic_error("couldn't convert matador::time into destination type");
}

void convert(std::string &dest, const time &source);

template < typename SourceType >
void convert(time &/*dest*/, SourceType &/*source*/)
{
  throw std::logic_error("couldn't convert value into time");
}

template < typename DestType >
void convert(DestType &/*dest*/, const date &/*data*/)
{
  throw std::logic_error("couldn't convert matador::date into destination type");
}

}

#endif //MATADOR_CONVERT_HPP
