#ifndef QUERY_TO_VALUE_HPP
#define QUERY_TO_VALUE_HPP

#include <charconv>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace matador::sql {

void to_value(bool &value, const char *str);

template < class Type >
void to_value(Type &value, const char *str, std::enable_if_t<std::is_integral_v<Type> && std::is_signed_v<Type>>* = nullptr)
{
  const auto res = std::from_chars(str, str + strlen(str), value);

  if (res.ec == std::errc::invalid_argument) {
    throw std::logic_error("failed to convert value to signed number: no digits were found");
  }
  if (res.ec == std::errc::result_out_of_range) {
    throw std::logic_error(std::string("out of range! res.ptr distance: ") + std::to_string(res.ptr - str));
  }
}

template < class Type >
void to_value(Type &value, const char *str, std::enable_if_t<std::is_integral_v<Type> && std::is_unsigned_v<Type> && !std::is_same_v<Type, bool>>* = nullptr)
{
    const auto res = std::from_chars(str, str + strlen(str), value);

    if (res.ec == std::errc::invalid_argument) {
        throw std::logic_error("failed to convert value to unsigned number: no digits were found");
    }
    if (res.ec == std::errc::result_out_of_range) {
        throw std::logic_error(std::string("out of range! res.ptr distance: ") + std::to_string(res.ptr - str));
    }
}

template < class Type >
void to_value(Type &value, const char *str, std::enable_if_t<std::is_floating_point_v<Type>>* = nullptr)
{
  const auto res = std::from_chars(str, str + strlen(str), value, std::chars_format::general);

  if (res.ec == std::errc::invalid_argument) {
    throw std::logic_error("failed to convert value to floating point number: no digits were found");
  }
  if (res.ec == std::errc::result_out_of_range) {
    throw std::logic_error(std::string("out of range! res.ptr distance: ") + std::to_string(res.ptr - str));
  }
}

}

#endif //QUERY_TO_VALUE_HPP
