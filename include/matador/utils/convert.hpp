#ifndef MATADOR_CONVERT_HPP
#define MATADOR_CONVERT_HPP

#include "matador/utils/types.hpp"
#include "matador/utils/result.hpp"
#include "matador/utils/os.hpp"

#include <array>
#include <cerrno>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace matador::utils {

enum class conversion_error : uint8_t {
  Ok,
  NotConvertable,
  MissingData
};

enum class conversion_policy : uint8_t {
  Strict,
  Relax
};

namespace detail {

template <typename T>
using decay_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
struct is_char_array : std::false_type {};

template <std::size_t N>
struct is_char_array<char[N]> : std::true_type {};

template <std::size_t N>
struct is_char_array<const char[N]> : std::true_type {};

template <typename T>
inline constexpr bool is_char_array_v = is_char_array<decay_t<T>>::value;

template <typename T>
inline constexpr bool is_supported_string_source_v =
  std::is_same_v<decay_t<T>, std::string> ||
  std::is_same_v<decay_t<T>, std::string_view> ||
  std::is_same_v<decay_t<T>, const char*> ||
  std::is_same_v<decay_t<T>, char*> ||
  is_char_array_v<T>;

inline result<std::string_view, conversion_error> make_string_view(const std::string &source)
{
  if (source.empty()) {
    return failure(conversion_error::MissingData);
  }

  return ok(std::string_view{source});
}

inline result<std::string_view, conversion_error> make_string_view(std::string_view source)
{
  if (source.empty()) {
    return failure(conversion_error::MissingData);
  }

  return ok(source);
}

inline result<std::string_view, conversion_error> make_string_view(const char *source)
{
  if (source == nullptr || *source == '\0') {
    return failure(conversion_error::MissingData);
  }

  return ok(std::string_view{source});
}

template <std::size_t N>
result<std::string_view, conversion_error> make_string_view(const char (&source)[N])
{
  static_assert(N > 0U, "char array must contain at least the terminating null character");

  if (source[0] == '\0') {
    return failure(conversion_error::MissingData);
  }

  const auto length = std::char_traits<char>::length(source);
  return ok(std::string_view{source, length});
}

template <std::size_t N>
result<std::string_view, conversion_error> make_string_view(char (&source)[N])
{
  static_assert(N > 0U, "char array must contain at least the terminating null character");

  if (source[0] == '\0') {
    return failure(conversion_error::MissingData);
  }

  const auto length = std::char_traits<char>::length(source);
  return ok(std::string_view{source, length});
}
inline std::string to_lower_ascii(std::string_view source)
{
  std::string result;
  result.reserve(source.size());

  for (const auto ch : source) {
    result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  }

  return result;
}

inline result<bool, conversion_error> parse_bool(std::string_view source)
{
  const auto value = to_lower_ascii(source);

  if (value == "true" || value == "t" || value == "on" || value == "1") {
    return ok(true);
  }

  if (value == "false" || value == "f" || value == "off" || value == "0") {
    return ok(false);
  }

  return failure(conversion_error::NotConvertable);
}

template <typename DestType>
result<DestType, conversion_error> parse_integral(std::string_view source)
{
  static_assert(std::is_integral_v<DestType>, "DestType must be integral");

  if (source.empty()) {
    return failure(conversion_error::MissingData);
  }

  DestType value{};
  const auto *begin = source.data();
  const auto *end = source.data() + source.size();

  const auto [ptr, ec] = std::from_chars(begin, end, value, 10);
  if (ec != std::errc{} || ptr != end) {
    return failure(conversion_error::NotConvertable);
  }

  return ok<DestType>(value);
}

template <typename DestType>
result<DestType, conversion_error> parse_floating(std::string_view source)
{
  static_assert(std::is_floating_point_v<DestType>, "DestType must be floating point");

  if (source.empty()) {
    return failure(conversion_error::MissingData);
  }

  std::string buffer{source};

  char *end = nullptr;
  errno = 0;

  const auto parsed = std::strtold(buffer.c_str(), &end);
  if (end == buffer.c_str() || *end != '\0' || errno == ERANGE) {
    return failure(conversion_error::NotConvertable);
  }

  if (parsed < -static_cast<long double>(std::numeric_limits<DestType>::max()) ||
      parsed > static_cast<long double>(std::numeric_limits<DestType>::max())) {
    return failure(conversion_error::NotConvertable);
  }

  return ok<DestType>(static_cast<DestType>(parsed));
}

template <typename DestType, typename SourceType>
result<DestType, conversion_error> checked_integral_cast(SourceType source)
{
  static_assert(std::is_integral_v<DestType>, "DestType must be integral");
  static_assert(std::is_integral_v<SourceType>, "SourceType must be integral");

  using source_type = std::remove_cv_t<SourceType>;
  using dest_type = std::remove_cv_t<DestType>;

  if constexpr (std::is_same_v<dest_type, source_type>) {
    return ok<DestType>(source);
  } else if constexpr (std::is_signed_v<source_type> && std::is_signed_v<dest_type>) {
    const auto value = static_cast<std::intmax_t>(source);
    const auto min = static_cast<std::intmax_t>(std::numeric_limits<dest_type>::lowest());
    const auto max = static_cast<std::intmax_t>(std::numeric_limits<dest_type>::max());

    if (value < min || value > max) {
      return failure(conversion_error::NotConvertable);
    }

    return ok<DestType>(static_cast<DestType>(source));
  } else if constexpr (std::is_unsigned_v<source_type> && std::is_unsigned_v<dest_type>) {
    const auto value = static_cast<std::uintmax_t>(source);
    const auto max = static_cast<std::uintmax_t>(std::numeric_limits<dest_type>::max());

    if (value > max) {
      return failure(conversion_error::NotConvertable);
    }

    return ok<DestType>(static_cast<DestType>(source));
  } else if constexpr (std::is_signed_v<source_type> && std::is_unsigned_v<dest_type>) {
    if (source < 0) {
      return failure(conversion_error::NotConvertable);
    }

    const auto value = static_cast<std::uintmax_t>(source);
    const auto max = static_cast<std::uintmax_t>(std::numeric_limits<dest_type>::max());

    if (value > max) {
      return failure(conversion_error::NotConvertable);
    }

    return ok<DestType>(static_cast<DestType>(source));
  } else {
    const auto value = static_cast<std::uintmax_t>(source);
    const auto max = static_cast<std::uintmax_t>(std::numeric_limits<dest_type>::max());

    if (value > max) {
      return failure(conversion_error::NotConvertable);
    }

    return ok<DestType>(static_cast<DestType>(source));
  }
}

template <typename DestType, typename SourceType>
result<DestType, conversion_error> checked_float_to_integral_cast(
  SourceType source,
  conversion_policy policy)
{
  static_assert(std::is_integral_v<DestType>, "DestType must be integral");
  static_assert(std::is_floating_point_v<SourceType>, "SourceType must be floating point");

  if (!std::isfinite(source)) {
    return failure(conversion_error::NotConvertable);
  }

  if (policy == conversion_policy::Strict && std::trunc(source) != source) {
    return failure(conversion_error::NotConvertable);
  }

  if (source < static_cast<SourceType>(std::numeric_limits<DestType>::lowest()) ||
      source > static_cast<SourceType>(std::numeric_limits<DestType>::max())) {
    return failure(conversion_error::NotConvertable);
  }

  return ok<DestType>(static_cast<DestType>(source));
}

template <typename DestType, typename SourceType>
result<DestType, conversion_error> checked_integral_to_float_cast(SourceType source)
{
  static_assert(std::is_floating_point_v<DestType>, "DestType must be floating point");
  static_assert(std::is_integral_v<SourceType>, "SourceType must be integral");

  const auto value = static_cast<DestType>(source);

  if (!std::isfinite(value)) {
    return failure(conversion_error::NotConvertable);
  }

  return ok<DestType>(value);
}

template <typename DestType, typename SourceType>
result<DestType, conversion_error> checked_float_cast(SourceType source)
{
  static_assert(std::is_floating_point_v<DestType>, "DestType must be floating point");
  static_assert(std::is_floating_point_v<SourceType>, "SourceType must be floating point");

  if (!std::isfinite(source)) {
    return failure(conversion_error::NotConvertable);
  }

  if (source < -static_cast<SourceType>(std::numeric_limits<DestType>::max()) ||
      source > static_cast<SourceType>(std::numeric_limits<DestType>::max())) {
    return failure(conversion_error::NotConvertable);
  }

  return ok<DestType>(static_cast<DestType>(source));
}

template <typename DestType, typename SourceType>
result<DestType, conversion_error> checked_numeric_cast(
  SourceType source,
  conversion_policy policy)
{
  if constexpr (std::is_same_v<DestType, SourceType>) {
    return ok<DestType>(source);
  } else if constexpr (std::is_same_v<DestType, bool>) {
    return ok<DestType>(source != SourceType{});
  } else if constexpr (std::is_same_v<SourceType, bool>) {
    return ok<DestType>(source ? DestType{1} : DestType{0});
  } else if constexpr (std::is_integral_v<DestType> && std::is_integral_v<SourceType>) {
    if (policy == conversion_policy::Relax) {
      return ok<DestType>(static_cast<DestType>(source));
    }

    return checked_integral_cast<DestType>(source);
  } else if constexpr (std::is_integral_v<DestType> && std::is_floating_point_v<SourceType>) {
    if (policy == conversion_policy::Relax) {
      return ok<DestType>(static_cast<DestType>(source));
    }

    return checked_float_to_integral_cast<DestType>(source, policy);
  } else if constexpr (std::is_floating_point_v<DestType> && std::is_integral_v<SourceType>) {
    return checked_integral_to_float_cast<DestType>(source);
  } else if constexpr (std::is_floating_point_v<DestType> && std::is_floating_point_v<SourceType>) {
    return checked_float_cast<DestType>(source);
  } else {
    return failure(conversion_error::NotConvertable);
  }
}

template <typename SourceType>
result<std::string, conversion_error> arithmetic_to_string(SourceType source)
{
  std::array<char, 128> buffer{};

  if constexpr (std::is_floating_point_v<SourceType>) {
    const auto [ptr, ec] = std::to_chars(
      buffer.data(),
      buffer.data() + buffer.size(),
      source,
      std::chars_format::general);

    if (ec == std::errc{}) {
      return ok(std::string(buffer.data(), ptr));
    }
  } else if constexpr (std::is_same_v<SourceType, bool>) {
    return ok<std::string>(source ? "true" : "false");
  } else {
    const auto [ptr, ec] = std::to_chars(
      buffer.data(),
      buffer.data() + buffer.size(),
      source,
      10);

    if (ec == std::errc{}) {
      return ok(std::string(buffer.data(), ptr));
    }
  }

  return failure(conversion_error::NotConvertable);
}

template <typename UIntType>
bool parse_fixed_unsigned(
  const std::string_view source,
  const std::size_t offset,
  const std::size_t length,
  UIntType &value)
{
  static_assert(std::is_unsigned_v<UIntType>, "UIntType must be unsigned");

  if (offset + length > source.size()) {
    return false;
  }

  const auto part = source.substr(offset, length);

  if (part.size() != length) {
    return false;
  }

  const auto *begin = part.data();
  const auto *end = part.data() + part.size();

  UIntType parsed{};
  const auto [ptr, ec] = std::from_chars(begin, end, parsed, 10);

  if (ec != std::errc{} || ptr != end) {
    return false;
  }

  value = parsed;
  return true;
}

inline bool is_leap_year(const uint32_t year)
{
  return (year % 4U == 0U && year % 100U != 0U) || year % 400U == 0U;
}

inline uint8_t days_in_month(const uint32_t year, const uint8_t month)
{
  switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return 31;
    case 4:
    case 6:
    case 9:
    case 11:
      return 30;
    case 2:
      return is_leap_year(year) ? 29 : 28;
    default:
      return 0;
  }
}

inline result<date_type_t, conversion_error> parse_date(const std::string_view source)
{
  if (source.size() != 10U || source[4] != '-' || source[7] != '-') {
    return failure(conversion_error::NotConvertable);
  }

  uint32_t year{};
  uint32_t month{};
  uint32_t day{};

  if (!parse_fixed_unsigned(source, 0, 4, year) ||
      !parse_fixed_unsigned(source, 5, 2, month) ||
      !parse_fixed_unsigned(source, 8, 2, day)) {
    return failure(conversion_error::NotConvertable);
  }

  if (month < 1U || month > 12U) {
    return failure(conversion_error::NotConvertable);
  }

  const auto max_day = days_in_month(year, static_cast<uint8_t>(month));
  if (day < 1U || day > max_day) {
    return failure(conversion_error::NotConvertable);
  }

  return ok(
    date_type_t{
      static_cast<int32_t>(year),
      static_cast<uint8_t>(month),
      static_cast<uint8_t>(day)
    });
}

inline result<time_type_t, conversion_error> parse_time(const std::string_view source)
{
  if (source.size() != 8U || source[2] != ':' || source[5] != ':') {
    return failure(conversion_error::NotConvertable);
  }

  uint32_t hour{};
  uint32_t minute{};
  uint32_t second{};

  if (!parse_fixed_unsigned(source, 0, 2, hour) ||
      !parse_fixed_unsigned(source, 3, 2, minute) ||
      !parse_fixed_unsigned(source, 6, 2, second)) {
    return failure(conversion_error::NotConvertable);
  }

  if (hour > 23U || minute > 59U || second > 59U) {
    return failure(conversion_error::NotConvertable);
  }

  return ok(
    time_type_t{
      static_cast<uint8_t>(hour),
      static_cast<uint8_t>(minute),
      static_cast<uint8_t>(second),
      0
    });
}

inline result<timestamp_type_t, conversion_error> parse_timestamp(const std::string_view source)
{
  if (source.size() != 19U || source[10] != ' ') {
    return failure(conversion_error::NotConvertable);
  }

  const auto date = parse_date(source.substr(0, 10));
  if (date.is_error()) {
    return failure(date.err());
  }

  const auto time = parse_time(source.substr(11, 8));
  if (time.is_error()) {
    return failure(time.err());
  }

  std::tm parsed{};
  parsed.tm_year = date->year - 1900;
  parsed.tm_mon = static_cast<int>(date->month) - 1;
  parsed.tm_mday = static_cast<int>(date->day);
  parsed.tm_hour = static_cast<int>(time->hour);
  parsed.tm_min = static_cast<int>(time->minute);
  parsed.tm_sec = static_cast<int>(time->second);
  parsed.tm_isdst = -1;

  const auto timestamp = std::mktime(&parsed);
  if (timestamp == static_cast<std::time_t>(-1)) {
    return failure(conversion_error::NotConvertable);
  }

  return ok(std::chrono::system_clock::from_time_t(timestamp));
}

inline result<std::string, conversion_error> date_to_string(const date_type_t &source)
{
  std::ostringstream stream;
  stream << std::setfill('0')
         << std::setw(4) << source.year << '-'
         << std::setw(2) << static_cast<unsigned>(source.month) << '-'
         << std::setw(2) << static_cast<unsigned>(source.day);

  return ok(stream.str());
}

inline result<std::string, conversion_error> time_to_string(const time_type_t &source)
{
  std::ostringstream stream;
  stream << std::setfill('0')
         << std::setw(2) << static_cast<unsigned>(source.hour) << ':'
         << std::setw(2) << static_cast<unsigned>(source.minute) << ':'
         << std::setw(2) << static_cast<unsigned>(source.second);

  return ok(stream.str());
}

inline result<std::string, conversion_error> timestamp_to_string(const timestamp_type_t &source)
{
  const auto time = std::chrono::system_clock::to_time_t(source);

  std::tm parsed{};
  matador::os::localtime(time, parsed);

  std::ostringstream stream;
  stream << std::put_time(&parsed, "%Y-%m-%d %H:%M:%S");

  if (stream.fail()) {
    return failure(conversion_error::NotConvertable);
  }

  return ok(stream.str());
}

template <typename SourceType>
result<blob_type_t, conversion_error> trivial_to_blob(const SourceType &source)
{
  static_assert(std::is_trivially_copyable_v<SourceType>, "SourceType must be trivially copyable");

  blob_type_t blob(sizeof(SourceType));
  std::memcpy(blob.data(), &source, sizeof(SourceType));

  return ok(std::move(blob));
}

inline result<blob_type_t, conversion_error> string_to_blob(const std::string &source)
{
  return ok(blob_type_t(source.begin(), source.end()));
}

inline result<blob_type_t, conversion_error> string_view_to_blob(std::string_view source)
{
  return ok(blob_type_t(source.begin(), source.end()));
}

inline result<blob_type_t, conversion_error> string_to_blob(const char *source)
{
  if (source == nullptr) {
    return failure(conversion_error::MissingData);
  }

  return ok(blob_type_t(source, source + std::strlen(source)));
}

template <typename DestType>
result<DestType, conversion_error> blob_to_trivial(const blob_type_t &source)
{
  static_assert(std::is_trivially_copyable_v<DestType>, "DestType must be trivially copyable");
  DestType value{};
  const auto bytes_to_copy = std::min(source.size(), sizeof(DestType));

  if (bytes_to_copy == 0U) {
    return failure(conversion_error::MissingData);
  }

  std::memcpy(&value, source.data(), bytes_to_copy);

  return ok<DestType>(value);
}

template <typename DestType, typename SourceType>
struct converter {
  static result<DestType, conversion_error> convert(
    const SourceType &source,
    conversion_policy policy)
  {
    using dest_type = decay_t<DestType>;
    using source_type = decay_t<SourceType>;

    if constexpr (std::is_same_v<dest_type, source_type>) {
      return ok<DestType>(source);
    } else if constexpr (std::is_arithmetic_v<dest_type> && std::is_arithmetic_v<source_type>) {
      return checked_numeric_cast<DestType>(source, policy);
    } else if constexpr (std::is_same_v<dest_type, std::string> && std::is_arithmetic_v<source_type>) {
      return arithmetic_to_string(source);
    } else if constexpr (std::is_same_v<dest_type, std::string> && std::is_same_v<source_type, const char*>) {
      if (source == nullptr) {
        return ok(std::string{});
      }

      return ok(std::string{source});
    } else if constexpr (std::is_same_v<dest_type, std::string> && std::is_same_v<source_type, date_type_t>) {
      return date_to_string(source);
    } else if constexpr (std::is_same_v<dest_type, std::string> && std::is_same_v<source_type, time_type_t>) {
      return time_to_string(source);
    } else if constexpr (std::is_same_v<dest_type, std::string> && std::is_same_v<source_type, timestamp_type_t>) {
      return timestamp_to_string(source);
    } else if constexpr (std::is_same_v<dest_type, bool> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_bool(*view);
    } else if constexpr (std::is_integral_v<dest_type> && !std::is_same_v<dest_type, bool> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_integral<DestType>(*view);
    } else if constexpr (std::is_floating_point_v<dest_type> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_floating<DestType>(*view);
    } else if constexpr (std::is_same_v<dest_type, date_type_t> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_date(*view);
    } else if constexpr (std::is_same_v<dest_type, time_type_t> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_time(*view);
    } else if constexpr (std::is_same_v<dest_type, timestamp_type_t> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return parse_timestamp(*view);
    } else if constexpr (std::is_same_v<dest_type, blob_type_t> && std::is_same_v<source_type, blob_type_t>) {
      return ok<blob_type_t>(source);
    } else if constexpr (std::is_same_v<dest_type, blob_type_t> && is_supported_string_source_v<source_type>) {
      const auto view = make_string_view(source);
      if (view.is_error()) {
        return failure(view.err());
      }

      return string_view_to_blob(*view);
    } else if constexpr (std::is_same_v<dest_type, blob_type_t> && std::is_trivially_copyable_v<source_type>) {
      return trivial_to_blob(source);
    } else if constexpr (!std::is_same_v<dest_type, blob_type_t> &&
                         std::is_same_v<source_type, blob_type_t> &&
                         std::is_trivially_copyable_v<dest_type>) {
      return blob_to_trivial<DestType>(source);
    } else if constexpr (std::is_same_v<dest_type, time_type_t> && std::is_same_v<source_type, timestamp_type_t>) {
      const auto time = std::chrono::system_clock::to_time_t(source);

      std::tm parsed{};
      matador::os::localtime(time, parsed);

      return ok<time_type_t>(
        time_type_t{
          static_cast<uint8_t>(parsed.tm_hour),
          static_cast<uint8_t>(parsed.tm_min),
          static_cast<uint8_t>(parsed.tm_sec),
          0
        });
    } else if constexpr (std::is_same_v<dest_type, date_type_t> && std::is_same_v<source_type, timestamp_type_t>) {
      const auto time = std::chrono::system_clock::to_time_t(source);

      std::tm parsed{};
      matador::os::localtime(time, parsed);

      return ok<date_type_t>(
        date_type_t{
          parsed.tm_year + 1900,
          static_cast<uint8_t>(parsed.tm_mon + 1),
          static_cast<uint8_t>(parsed.tm_mday)
        });
    } else {
      return failure(conversion_error::NotConvertable);
    }
  }
};

} // namespace detail

template <typename DestType, typename SourceType>
result<DestType, conversion_error> to(
  const SourceType &source,
  conversion_policy policy = conversion_policy::Strict)
{
  return detail::converter<DestType, detail::decay_t<SourceType>>::convert(source, policy);
}

} // namespace matador::utils

#endif //MATADOR_CONVERT_HPP