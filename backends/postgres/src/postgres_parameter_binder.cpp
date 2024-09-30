#include "postgres_parameter_binder.h"

#include "matador/utils/string.hpp"

namespace matador::backends::postgres {
namespace detail {
template<class T>
void bind_value(std::vector<std::string> &strings, std::vector<const char *> &params, size_t index, T &x) {
  strings[index] = std::to_string(x);
  params[index] = strings[index].c_str();
}

template<>
void bind_value(std::vector<std::string> &strings, std::vector<const char *> &params, size_t index, char &x) {
  strings[index] = std::to_string(x);
  params[index] = strings[index].data();
}

template<>
void bind_value(std::vector<std::string> &strings, std::vector<const char *> &params, size_t index, unsigned char &x) {
  strings[index] = std::to_string(x);
  params[index] = strings[index].data();
}

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

postgres_parameter_binder::postgres_parameter_binder(size_t size)
  : strings_(size)
    , params_(size) {
}

void postgres_parameter_binder::write_value(size_t pos, const char &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const short &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const int &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const long &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const long long int &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const unsigned char &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const unsigned short &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const unsigned int &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const unsigned long &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const unsigned long long int &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const bool &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const float &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const double &x) {
  detail::bind_value(strings_, params_, pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const char *x) {
  params_[pos] = x;
}

void postgres_parameter_binder::write_value(size_t pos, const char *x, size_t size) {
  params_[pos] = x;
}

void postgres_parameter_binder::write_value(size_t pos, const std::string &x) {
  strings_[pos] = x;
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::write_value(size_t pos, const std::string &x, size_t size) {
  write_value(pos, x);
}

void postgres_parameter_binder::write_value(size_t pos, const time &x) {
  strings_[pos] = matador::utils::to_string(x, "%Y-%m-%d %T.%f");
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::write_value(size_t pos, const date &x) {
  strings_[pos] = matador::utils::to_string(x, utils::date_format::ISO8601);
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::write_value(size_t pos, const utils::blob &x) {
  params_[pos] = "";
}

const std::vector<const char *> &postgres_parameter_binder::params() const {
  return params_;
}
}
