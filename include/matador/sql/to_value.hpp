#ifndef QUERY_TO_VALUE_HPP
#define QUERY_TO_VALUE_HPP

#include <cerrno>
#include <climits>
#include <cstring>
#include <cfloat>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

namespace matador::sql {

template < class Type >
void to_value(Type &value, const char *str, typename std::enable_if<std::is_integral<Type>::value && std::is_signed<Type>::value>::type* = nullptr)
{
  if (strlen(str) == 0) {
    return;
  }
  char *end;
  errno = 0;
  auto result = strtoll(str, &end, 10);

  // Check for various possible errors
  if ((errno == ERANGE && (result == LLONG_MAX || result == LLONG_MIN)) || (errno != 0 && result == 0)) {
    throw std::logic_error(strerror(errno));
    // Handle error
  } else if (end == str) {
    // No digits found
    throw std::logic_error("failed to convert value to signed number: no digits were found");
  }

  value = static_cast<Type>(result);
}

template < class Type >
void to_value(Type &value, const char *str, typename std::enable_if<std::is_integral<Type>::value && std::is_unsigned<Type>::value>::type* = nullptr)
{
  if (strlen(str) == 0) {
    return;
  }

  char *end;
  errno = 0;
  auto result = strtoull(str, &end, 10);

  // Check for various possible errors
  if ((errno == ERANGE && (result == LLONG_MAX || result == LLONG_MIN)) || (errno != 0 && result == 0)) {
    throw std::logic_error(strerror(errno));
    // Handle error
  } else if (end == str) {
    // No digits found
    throw std::logic_error("failed to convert value to unsigned number: no digits were found");
  }

  value = static_cast<Type>(result);
}

template < class Type >
void to_value(Type &value, const char *str, typename std::enable_if<std::is_floating_point<Type>::value>::type* = nullptr)
{
  if (strlen(str) == 0) {
    return;
  }

  char *end;
  errno = 0;
  auto result = strtold(str, &end);

  // Check for various possible errors
  if ((errno == ERANGE && (result == LDBL_MAX || result == LDBL_MIN)) || (errno != 0 && result == 0)) {
    throw std::logic_error(strerror(errno));
    // Handle error
  } else if (end == str) {
    // No digits found
    throw std::logic_error("failed to convert value to floating point number: no digits were found");
  }

  value = static_cast<Type>(result);
}

}

#endif //QUERY_TO_VALUE_HPP
