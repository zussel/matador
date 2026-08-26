#include "matador/utils/types.hpp"

namespace matador::utils {
// constexpr bool operator==(const date_type_t& a, const date_type_t& b) noexcept

constexpr bool operator!=(const date_type_t &a, const date_type_t &b) noexcept {
  return !(a == b);
}

constexpr bool operator<(const date_type_t &a, const date_type_t &b) noexcept {
  return (a.year  < b.year)  ||
         (a.year  == b.year  && a.month < b.month) ||
         (a.year  == b.year  && a.month == b.month && a.day < b.day);
}

constexpr bool operator>(const date_type_t &a, const date_type_t &b) noexcept {
  return b < a;
}

constexpr bool operator<=(const date_type_t &a, const date_type_t &b) noexcept {
  return !(b < a);
}

constexpr bool operator>=(const date_type_t &a, const date_type_t &b) noexcept {
  return !(a < b);
}

constexpr bool operator!=(const time_type_t &a, const time_type_t &b) noexcept {
  return !(a == b);
}

constexpr bool operator<(const time_type_t &a, const time_type_t &b) noexcept {
  return (a.hour   < b.hour)   ||
         (a.hour   == b.hour   && a.minute < b.minute) ||
         (a.hour   == b.hour   && a.minute == b.minute && a.second < b.second) ||
         (a.hour   == b.hour   && a.minute == b.minute && a.second == b.second &&
          a.microsecond < b.microsecond);
}

constexpr bool operator>(const time_type_t &a, const time_type_t &b) noexcept {
  return b < a;
}

constexpr bool operator<=(const time_type_t &a, const time_type_t &b) noexcept {
  return !(b < a);
}

constexpr bool operator>=(const time_type_t &a, const time_type_t &b) noexcept {
  return !(a < b);
}

void initialize_by_basic_type(const basic_type type, database_type &val) {
  switch (type) {
    case basic_type::Int8:
      val.emplace<int8_t>();
      break;
    case basic_type::Int16:
      val.emplace<int16_t>();
      break;
    case basic_type::Int32:
      val.emplace<int32_t>();
      break;
    case basic_type::Int64:
      val.emplace<int64_t>();
      break;
    case basic_type::UInt8:
      val.emplace<uint8_t>();
      break;
    case basic_type::UInt16:
      val.emplace<uint16_t>();
      break;
    case basic_type::UInt32:
      val.emplace<uint32_t>();
      break;
    case basic_type::UInt64:
      val.emplace<uint64_t>();
      break;
    case basic_type::Boolean:
      val.emplace<bool>();
      break;
    case basic_type::Float:
      val.emplace<float>();
      break;
    case basic_type::Double:
      val.emplace<double>();
      break;
    case basic_type::Varchar:
    case basic_type::Text:
      val.emplace<std::string>();
      break;
    case basic_type::Date:
      val.emplace<date_type_t>();
      break;
    case basic_type::Time:
      val.emplace<time_type_t>();
      break;
    case basic_type::DateTime:
      val.emplace<timestamp_type_t>();
      break;
    case basic_type::Blob:
      val.emplace<blob_type_t>();
      break;
    default:
      val.emplace<nullptr_t>();
  }
}

}