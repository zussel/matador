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
, formats(size)
{}

postgres_parameter_binder::postgres_parameter_binder(size_t size)
: bind_data_(size)
{}

void postgres_parameter_binder::write_value(const size_t pos, const char &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const short &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const int &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const long &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const long long int &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const unsigned char &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const unsigned short &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const unsigned int &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const unsigned long &x) {
  detail::bind_value(bind_data_, pos, x);
}

void postgres_parameter_binder::write_value(const size_t pos, const unsigned long long int &x) {
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

void postgres_parameter_binder::write_value(const size_t pos, const time &x) {
    bind_data_.strings[pos] = utils::to_string(x, "%Y-%m-%d %T.%f");
    bind_data_.values[pos] = bind_data_.strings[pos].data();
    bind_data_.lengths[pos] = static_cast<int>(bind_data_.strings[pos].size());
    bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const date &x) {
    bind_data_.strings[pos] = utils::to_string(x, utils::date_format::ISO8601);
    bind_data_.values[pos] = bind_data_.strings[pos].data();
    bind_data_.lengths[pos] = static_cast<int>(bind_data_.strings[pos].size());
    bind_data_.formats[pos] = 0;
}

void postgres_parameter_binder::write_value(const size_t pos, const utils::blob &x) {
  bind_data_.bytes[pos] = x;
  bind_data_.values[pos] = reinterpret_cast<char*>(bind_data_.bytes[pos].data());
  bind_data_.lengths[pos] = static_cast<int>(bind_data_.bytes[pos].size());
  bind_data_.formats[pos] = 1;
}

void postgres_parameter_binder::write_value(const size_t /*pos*/, const utils::value &/*x*/, size_t /*size*/) {}

const postgres_parameter_binder::bind_data &postgres_parameter_binder::params() const {
  return bind_data_;
}

}
