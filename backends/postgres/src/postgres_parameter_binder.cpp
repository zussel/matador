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

void postgres_parameter_binder::bind(size_t pos, char i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, short i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, int i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, long i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, long long int i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, unsigned char i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, unsigned short i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, unsigned int i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, unsigned long i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, unsigned long long int i) {
  detail::bind_value(strings_, params_, pos, i);
}

void postgres_parameter_binder::bind(size_t pos, bool b) {
  detail::bind_value(strings_, params_, pos, b);
}

void postgres_parameter_binder::bind(size_t pos, float d) {
  detail::bind_value(strings_, params_, pos, d);
}

void postgres_parameter_binder::bind(size_t pos, double d) {
  detail::bind_value(strings_, params_, pos, d);
}

void postgres_parameter_binder::bind(size_t pos, const char *str) {
  params_[pos] = str;
}

void postgres_parameter_binder::bind(size_t pos, const char *str, size_t size) {
  params_[pos] = str;
}

void postgres_parameter_binder::bind(size_t pos, const std::string &str) {
  strings_[pos] = str;
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::bind(size_t pos, const std::string &str, size_t size) {
  bind(pos, str);
}

void postgres_parameter_binder::bind(size_t pos, const time &x) {
  strings_[pos] = matador::utils::to_string(x, "%Y-%m-%d %T.%f");
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::bind(size_t pos, const date &date) {
  strings_[pos] = matador::utils::to_string(date, utils::date_format::ISO8601);
  params_[pos] = strings_[pos].c_str();
}

void postgres_parameter_binder::bind(size_t pos, const utils::blob &blob) {
  params_[pos] = "";
}

const std::vector<const char *> &postgres_parameter_binder::params() const {
  return params_;
}
}
