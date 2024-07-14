#ifndef MATADOR_VALUE_PROCESSOR_HPP
#define MATADOR_VALUE_PROCESSOR_HPP

#include "matador/utils/serializer.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/sql/value.hpp"

#include <sstream>

namespace matador {

/// @cond MATADOR_DEV

class basic_dialect;

namespace detail {

class value_processor
{
public:
  explicit value_processor(serializer &serializer);

  void apply(const char *id, matador::value &val, const field_attributes &attr);

  void operator()(char &x) { process(x, attributes_); }
  void operator()(short &x) { process(x, attributes_); }
  void operator()(int &x) { process(x, attributes_); }
  void operator()(long &x) { process(x, attributes_); }
  void operator()(long long &x) { process(x, attributes_); }
  void operator()(unsigned char &x) { process(x, attributes_); }
  void operator()(unsigned short &x) { process(x, attributes_); }
  void operator()(unsigned int &x) { process(x, attributes_); }
  void operator()(unsigned long &x) { process(x, attributes_); }
  void operator()(unsigned long long &x) { process(x, attributes_); }
  void operator()(bool &x) { process(x, attributes_); }
  void operator()(float &x) { process(x, attributes_); }
  void operator()(double &x) { process(x, attributes_); }
  void operator()(const char *x) { process(x, attributes_); }
  void operator()(std::string &x) { process(x, attributes_); }
  void operator()(matador::date &x) { process(x, attributes_); }
  void operator()(matador::time &x) { process(x, attributes_); }
  void operator()(utils::blob &x) { process(x, attributes_); }
  void operator()(sql::placeholder &/*x*/) {}

private:
  template < class T >
  void process(T &val, const field_attributes &attr, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr)
  {
    serializer_.on_attribute(id_, val, attr);
  }

  template < class T >
  void process(T &val, const field_attributes &attr, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
  {
    serializer_.on_attribute(id_, val, attr);
  }

  void process(std::string &val, const field_attributes &attr);
  void process(const char *val, const field_attributes &attr);
  void process(time &val, const field_attributes &attr);
  void process(date &val, const field_attributes &attr);
  void process(utils::blob &val, const field_attributes &attr);

private:
  field_attributes attributes_;
  serializer &serializer_;
  const char *id_ = nullptr;
};

class value_to_string_processor
{
public:
  enum class style {
    Compact, Safe
  };
  explicit value_to_string_processor(const basic_dialect &dialect);

  std::string to_string(const matador::value &v);
  std::string to_safe_string(const matador::value &v);

  void operator()(char &x) { process(x); }
  void operator()(short &x) { process(x); }
  void operator()(int &x) { process(x); }
  void operator()(long &x) { process(x); }
  void operator()(long long &x) { process(x); }
  void operator()(unsigned char &x) { process(x); }
  void operator()(unsigned short &x) { process(x); }
  void operator()(unsigned int &x) { process(x); }
  void operator()(unsigned long &x) { process(x); }
  void operator()(unsigned long long &x) { process(x); }
  void operator()(bool &x) { process(x); }
  void operator()(float &x) { process(x); }
  void operator()(double &x) { process(x); }
  void operator()(const char *x) { process(x); }
  void operator()(std::string &x) { process(x); }
  void operator()(matador::date &x) { process(x); }
  void operator()(matador::time &x) { process(x); }
  void operator()(utils::blob &x) { process(x); }
  void operator()(sql::placeholder &/*x*/) {}

private:
  template < class T >
  void process(T &val, typename std::enable_if<
    std::is_integral<T>::value &&
    !std::is_same<char, T>::value &&
    !std::is_same<char*, T>::value>::type* = nullptr)
  {
    std::stringstream ss;
    ss << val;
    result_ = ss.str();
  }

  template < class T >
  void process(T &val, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
  {
    std::stringstream ss;
    ss << val;
    result_ = ss.str();
  }

  void process(std::string &val);
  void process(char &val);
  void process(unsigned char &val);
  void process(char *val);
  void process(const char *val);
  void process(time &val);
  void process(date &val);
  void process(utils::blob &val);

private:
  const basic_dialect &dialect_;
  std::string result_;
  style style_{style::Compact};
};

}

/// @endcond

}
#endif //MATADOR_VALUE_PROCESSOR_HPP
