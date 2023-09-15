#include "matador/db/postgresql/postgresql_parameter_binder.hpp"

#include "matador/utils/string.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

namespace matador {

namespace postgresql {

template < class T >
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, T &x)
{
  strings[index] = std::to_string(x);
  params[index] = strings[index].c_str();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, char &x)
{
  strings[index] = std::to_string(x);
  params[index] = strings[index].data();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, unsigned char &x)
{
  strings[index] = std::to_string(x);
  params[index] = strings[index].data();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, const matador::date &x)
{
  strings[index] = matador::to_string(x, date_format::ISO8601);
  params[index] = strings[index].c_str();
  ++index;
}

template <>
void bind_value(std::vector<std::string> &strings, std::vector<const char*> &params, size_t &index, const matador::time &x)
{
  strings[index] = matador::to_string(x, "%Y-%m-%d %T.%f");
  params[index] = strings[index].c_str();
  ++index;
}

postgresql_parameter_binder::postgresql_parameter_binder(size_t bind_var_size)
{
  params_.resize(bind_var_size);
  strings_.resize(bind_var_size);
}

void postgresql_parameter_binder::reset()
{
  index_ = 0;
}

void postgresql_parameter_binder::bind(char i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(short i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(int i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(long i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(long long i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(unsigned char i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(unsigned short i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(unsigned int i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(unsigned long i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(unsigned long long i, size_t index)
{
  bind_value(strings_, params_, index, i);
}

void postgresql_parameter_binder::bind(bool b, size_t index)
{
  bind_value(strings_, params_, index, b);
}

void postgresql_parameter_binder::bind(float d, size_t index)
{
  bind_value(strings_, params_, index, d);
}

void postgresql_parameter_binder::bind(double d, size_t index)
{
  bind_value(strings_, params_, index, d);
}

void postgresql_parameter_binder::bind(const char *x, size_t, size_t index)
{
  params_[index] = x;
}

void postgresql_parameter_binder::bind(const std::string &x, size_t index)
{
  strings_[index] = x;
  params_[index] = strings_[index].c_str();
  ++index;
}

void postgresql_parameter_binder::bind(const std::string &x, size_t, size_t index)
{
  strings_[index] = x;
  params_[index] = strings_[index].c_str();
  ++index;
}

void postgresql_parameter_binder::bind(const matador::time &x, size_t index)
{
  bind_value(strings_, params_, index, x);
}

void postgresql_parameter_binder::bind(const matador::date &x, size_t index)
{
  bind_value(strings_, params_, index, x);
}

const std::vector<const char *> &postgresql_parameter_binder::params() const
{
  return params_;
}

}
}