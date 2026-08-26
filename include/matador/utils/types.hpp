#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP

#include <chrono>
#include <cstdint>
#include <variant>
#include <vector>

namespace matador::utils {
/**
 * @brief Enumeration type of all supported basic data types
 */
enum class basic_type : uint8_t {
  Int8 = 0,           /*!< Data type int8 */
  Int16,              /*!< Data type int16 */
  Int32,              /*!< Data type int32 */
  Int64,              /*!< Data type int64 */
  UInt8,              /*!< Data type unsigned int8 */
  UInt16,             /*!< Data type unsigned int16 */
  UInt32,             /*!< Data type unsigned int32 */
  UInt64,             /*!< Data type unsigned int64 */
  Float,              /*!< Data type float */
  Double,             /*!< Data type double */
  Boolean,            /*!< Data type bool */
  Varchar,            /*!< Data type varchar */
  Text,               /*!< Data type text */
  Date,               /*!< Data type date */
  DateTime,           /*!< Data type datetime */
  Time,               /*!< Data type time */
  Blob,               /*!< Data type blob */
  Null,               /*!< Data type null */
  Unknown             /*!< Data type unknown */
};

using byte = unsigned char;
using blob_type_t = std::vector<byte>;
using timestamp_type_t = std::chrono::system_clock::time_point;

struct date_type_t;
struct time_type_t;

using database_type = std::variant<
  uint8_t, uint16_t, uint32_t, uint64_t,
  int8_t, int16_t, int32_t, int64_t,
  float, double,
  bool,
  const char*,
  std::string,
  blob_type_t,
  timestamp_type_t,
  date_type_t,
  time_type_t,
  nullptr_t>;

struct null_type_t {};

struct date_type_t {
  int32_t year{};
  uint8_t month{}; // 1-12
  uint8_t day{};
};

struct time_type_t {
  uint8_t hour{};
  uint8_t minute{};
  uint8_t second{};
  uint32_t microsecond{};
};

namespace detail {
template <typename T, typename Variant>
struct is_in_variant;

template <typename T, typename... Alts>
struct is_in_variant<T, std::variant<Alts...>>
  : std::bool_constant<(std::is_same_v<T, Alts> || ...)> {
};
}

template<typename T>
struct is_database_primitive : detail::is_in_variant<std::remove_cv_t<std::remove_reference_t<T>>, database_type> {
};

template <typename T>
inline constexpr bool is_database_primitive_v = is_database_primitive<T>::value;

void initialize_by_basic_type(basic_type type, database_type &val);


// Equality
constexpr bool operator==(const date_type_t& a, const date_type_t& b) noexcept {
  return a.year  == b.year &&
         a.month == b.month &&
         a.day   == b.day;
}

constexpr bool operator!=(const date_type_t& a, const date_type_t& b) noexcept;

// Ordering
constexpr bool operator<(const date_type_t& a, const date_type_t& b) noexcept;
constexpr bool operator>(const date_type_t& a, const date_type_t& b) noexcept;
constexpr bool operator<=(const date_type_t& a, const date_type_t& b) noexcept;
constexpr bool operator>=(const date_type_t& a, const date_type_t& b) noexcept;


// Equality
constexpr bool operator==(const time_type_t &a, const time_type_t &b) noexcept {
  return a.hour        == b.hour &&
         a.minute      == b.minute &&
         a.second      == b.second &&
         a.microsecond == b.microsecond;
}

constexpr bool operator!=(const time_type_t& a, const time_type_t& b) noexcept;

// Ordering
constexpr bool operator<(const time_type_t& a, const time_type_t& b) noexcept;
constexpr bool operator>(const time_type_t& a, const time_type_t& b) noexcept;
constexpr bool operator<=(const time_type_t& a, const time_type_t& b) noexcept;
constexpr bool operator>=(const time_type_t& a, const time_type_t& b) noexcept;

}

#endif //BASIC_TYPES_HPP
