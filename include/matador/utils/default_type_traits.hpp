#ifndef MATADOR_DEFAULT_TYPE_TRAITS_HPP
#define MATADOR_DEFAULT_TYPE_TRAITS_HPP

#include "matador/utils/type_traits.hpp"
#include "matador/utils/types.hpp"

#include <cstdint>
#include <string>

namespace matador::utils {
/// @cond MATADOR_DEV
template<>
struct data_type_traits<nullptr_t, void> {
  static basic_type type(std::size_t /*size*/) { return basic_type::Null; }
  static void read_value(value_reader &reader, const char *id, size_t index, nullptr_t &/*value*/,
                         size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, nullptr_t &/*value*/, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<int8_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Int8; }
  static void read_value(value_reader &reader, const char *id, size_t index, int8_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const int8_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<int16_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Int16; }
  static void read_value(value_reader &reader, const char *id, size_t index, int16_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const int16_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<int32_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Int32; }
  static void read_value(value_reader &reader, const char *id, size_t index, int32_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const int32_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<int64_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Int64; }
  static void read_value(value_reader &reader, const char *id, size_t index, int64_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const int64_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<uint8_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::UInt8; }
  static void read_value(value_reader &reader, const char *id, size_t index, uint8_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const uint8_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<uint16_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::UInt16; }
  static void read_value(value_reader &reader, const char *id, size_t index, uint16_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const uint16_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<uint32_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::UInt32; }
  static void read_value(value_reader &reader, const char *id, size_t index, uint32_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const uint32_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<uint64_t, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::UInt64; }
  static void read_value(value_reader &reader, const char *id, size_t index, uint64_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const uint64_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<bool, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Boolean; }
  static void read_value(value_reader &reader, const char *id, size_t index, bool &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const bool &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<float, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Float; }
  static void read_value(value_reader &reader, const char *id, size_t index, float &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const float &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<double, void> {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Double; }
  static void read_value(value_reader &reader, const char *id, size_t index, double &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, const double &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<const char *, void> {
  static basic_type type(const std::size_t size) { return size == 0 ? basic_type::Text : basic_type::Varchar; }
  static void read_value(value_reader &reader, const char *id, size_t index, const char *value, size_t size);
  static void bind_value(value_writer &binder, size_t index, const char *value, size_t size = 0);
};

template<>
struct data_type_traits<char *, void> {
  static basic_type type(const std::size_t size) { return size == 0 ? basic_type::Text : basic_type::Varchar; }
  static void read_value(value_reader &reader, const char *id, size_t index, char *value, size_t size);
  static void bind_value(value_writer &binder, size_t index, const char *value, size_t size = 0);
};

template<>
struct data_type_traits<char[], void> {
  static basic_type type(const std::size_t size) { return size == 0 ? basic_type::Text : basic_type::Varchar; }

  template<int N>
  static void read_value(value_reader &reader, const char *id, const size_t index, char (&value)[N], const size_t size) {
    data_type_traits<const char *>::read_value(reader, id, index, value, size);
  }

  template<int N>
  static void bind_value(value_writer &binder, const size_t index, char *value, const size_t size = 0) {
    data_type_traits<const char *>::bind_value(binder, index, value, size);
  }
};

template<>
struct data_type_traits<std::string, void> {
  static basic_type type(const std::size_t size) { return size == 0 ? basic_type::Text : basic_type::Varchar; }
  static void read_value(value_reader &reader, const char *id, size_t index, std::string &value, size_t size);
  static void bind_value(value_writer &binder, size_t index, std::string &value, size_t size = 0);
};

template<>
struct data_type_traits<blob_type_t, void> {
  static basic_type type(std::size_t /*size*/) { return basic_type::Blob; }
  static void read_value(value_reader &reader, const char *id, size_t index, utils::blob_type_t &value, size_t /*size*/  = 0);
  static void bind_value(value_writer &binder, size_t index, utils::blob_type_t &value, size_t /*size*/  = 0);
};

template<>
struct data_type_traits<date_type_t, void> {
  static basic_type type(std::size_t /*size*/) { return basic_type::Date; }
  static void read_value(value_reader &reader, const char *id, size_t index, date_type_t &value);
  static void bind_value(value_writer &binder, size_t index, date_type_t &value);
};

template<>
struct data_type_traits<time_type_t, void> {
  static basic_type type(std::size_t /*size*/) { return basic_type::Time; }
  static void read_value(value_reader &reader, const char *id, size_t index, time_type_t &value);
  static void bind_value(value_writer &binder, size_t index, time_type_t &value);
};

template<>
struct data_type_traits<timestamp_type_t, void> {
  static basic_type type(std::size_t /*size*/) { return basic_type::DateTime; }
  static void read_value(value_reader &reader, const char *id, size_t index, timestamp_type_t &value);
  static void bind_value(value_writer &binder, size_t index, timestamp_type_t &value);
};

template<typename EnumType>
struct data_type_traits<EnumType, std::enable_if_t<std::is_enum_v<EnumType> > > {
  static basic_type type(std::size_t /*size*/  = 0) { return basic_type::Int32; }

  static void read_value(value_reader &reader, const char *id, const size_t index, EnumType &value, const size_t size = 0) {
    data_type_traits<int>::read_value(reader, id, index, reinterpret_cast<int &>(value), size);
  }

  static void bind_value(value_writer &binder, const size_t index, EnumType &value, const size_t size = 0) {
    data_type_traits<int>::bind_value(binder, index, static_cast<int &>(value), size);
  }
};

/// @endcond
}
#endif //MATADOR_DEFAULT_TYPE_TRAITS_HPP
