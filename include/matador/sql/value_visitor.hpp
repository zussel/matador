#ifndef MATADOR_VALUE_VISITOR_HPP
#define MATADOR_VALUE_VISITOR_HPP

#include "matador/utils/any_visitor.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

#include <sstream>

namespace matador {

/// @cond MATADOR_DEV

class basic_dialect;

namespace detail {

class value_visitor
{
public:
  value_visitor();

  void apply(matador::any &a, const char *id, serializer &s);

private:
  template < class T >
  void process(T &val, typename std::enable_if<std::is_integral<T>::value>::type* = 0)
  {
    serializer_->on_attribute(id_, val);
  }

  template < class T >
  void process(T &val, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
  {
    serializer_->on_attribute(id_, val);
  }

  void process(std::string &val);
  void process(char *val);
  void process(time &val);
  void process(date &val);

private:
  matador::any_visitor visitor;
  serializer *serializer_ = nullptr;
  const char *id_ = nullptr;
};

class value_to_string_visitor
{
public:
  value_to_string_visitor();

  std::string to_string(matador::any &a);
  std::string to_safe_string(matador::any &a, const basic_dialect *d);

  template < class T >
  void process(T &val, typename std::enable_if<
    std::is_integral<T>::value &&
    !std::is_same<char, T>::value &&
    !std::is_same<char*, T>::value>::type* = 0)
  {
    std::stringstream ss;
    ss << val;
    result_ = ss.str();
  }

  template < class T >
  void process(T &val, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
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

private:
  matador::any_visitor visitor;
  const basic_dialect *dialect_ = nullptr;
  std::string result_;
};

}

/// @endcond

}
#endif //MATADOR_VALUE_VISITOR_HPP
