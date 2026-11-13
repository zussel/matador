#include "postgres_parameter_binder.h"

#include "matador/utils/string.hpp"

#include <cstring>

namespace matador::backends::postgres {
namespace detail {
template<class T>
void bind_value(postgres_parameter_binder::bind_data &data, const size_t index, const T &x) {
  data.strings[index] = std::to_string(x);
  data.values[index] = data.strings[index].c_str();
  data.lengths[index] = static_cast<int>(data.strings[index].size());
  data.formats[index] = 0;
}

// template<>
// void bind_value(postgres_parameter_binder::bind_data &data, size_t index, const char &x) {
//   data.strings[index] = std::to_string(x);
//   data.values[index] = data.strings[index].data();
//   data.formats[index] = 0;
// }

// template<>
// void bind_value(postgres_parameter_binder::bind_data &data, size_t index, const unsigned char &x) {
//   data.strings[index] = std::to_string(x);
//   data.values[index] = data.strings[index].data();
//   data.formats[index] = 0;
// }

//template <>
//void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, const matador::date &x)
//{
//  strings[index] = matador::to_string(x, date_format::ISO8601);
//  params[index] = strings[index].c_str();
//  ++index;
//}
//
//template <>
//void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, const matador::time &x)
//{
//  strings[index] = matador::to_string(x, "%Y-%m-%d %T.%f");
//  params[index] = strings[index].c_str();
//  ++index;
//}
}

postgres_parameter_binder::bind_data::bind_data(const size_t size)
: strings(size)
  , bytes(size)
  , values(size)
  , lengths(size)
  , formats(size) {
}

postgres_parameter_binder::postgres_parameter_binder(const size_t size)
: bind_data_(size) {
}

void postgres_parameter_binder::write_value(const size_t pos, const int8_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const int16_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const int32_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const int64_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const uint8_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const uint16_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const uint32_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const uint64_t &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const bool &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const float &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const double &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const char *x) {
  bind_data_.values[pos] = x;
  bind_data_.lengths[pos] = static_cast<int>(std::strlen(x));
  bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const char *x, size_t /*size*/) {
  bind_data_.values[pos] = x;
  bind_data_.lengths[pos] = static_cast<int>(std::strlen(x));
  bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const std::string &x) {
  bind_data_.values[pos] = x.data();
  bind_data_.lengths[pos] = static_cast<int>(x.size());
  bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const std::string &x, size_t /*size*/) {
  write_value(pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const utils::date_type_t &x) {
  char buf[11]; // "YYYY-MM-DD" + '\0'

  std::snprintf(buf, sizeof(buf), "%04d-%02u-%02u", x.year, static_cast<unsigned>(x.month), static_cast<unsigned>(x.day));
  bind_data_.strings[pos] = std::string{buf};
  bind_data_.values[pos] = bind_data_.strings[pos].data();
  bind_data_.lengths[pos] = static_cast<int>(bind_data_.strings[pos].size());
  bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const utils::time_type_t &x) {
  char buf[16]; // "HH:MM:SS.ffffff" + '\0'

  std::snprintf(buf, sizeof(buf), "%02u:%02u:%02u.%06u",
    static_cast<unsigned>(x.hour), static_cast<unsigned>(x.minute), static_cast<unsigned>(x.second), x.microsecond);

  bind_data_.strings[pos] = std::string{buf};
  bind_data_.values[pos] = bind_data_.strings[pos].data();
  bind_data_.lengths[pos] = static_cast<int>(bind_data_.strings[pos].size());
  bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(size_t pos, const utils::timestamp_type_t &x) {
}

void postgres_parameter_binder::write_value(const size_t pos, const utils::blob_type_t &x) {
  bind_data_.bytes[pos] = x;
  bind_data_.values[pos] = reinterpret_cast<char *>(bind_data_.bytes[pos].data());
  bind_data_.lengths[pos] = static_cast<int>(bind_data_.bytes[pos].size());
  bind_data_.formats[pos] = 1;
}

void postgres_parameter_binder::write_value(const size_t /*pos*/, const query::column_value &/*x*/, size_t /*size*/) {
}

const postgres_parameter_binder::bind_data &postgres_parameter_binder::params() const {
  return bind_data_;
}
}
