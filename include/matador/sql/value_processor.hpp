#ifndef MATADOR_VALUE_PROCESSOR_HPP
#define MATADOR_VALUE_PROCESSOR_HPP

#include "matador/utils/any_visitor.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"

#include "matador/sql/value.hpp"

#include <sstream>

namespace matador {

/// @cond MATADOR_DEV

struct value;
class basic_dialect;

namespace detail {

class value_visitor
{
public:
  using function = std::function<void(matador::value&, const field_attributes&)>; /**< Shortcut for the visitor callback function */

  template <typename T>
  void register_visitor(const std::function<void(T&, const field_attributes&)> &f) {
    fs.insert(std::make_pair(
      std::type_index(typeid(T)),
      function([f](matador::value &value, const field_attributes &attr) {
        f(value.value_._<T>(), attr);
      })
    ));
  }

  /**
   * @brief Applies the visitor pattern on an any object.
   * @param x The any object the pattern should be applied on
   * @return Returns true if a function could be applied.
   */
  bool visit(matador::value &value, const field_attributes &attr) {
    auto it = fs.find(value.value_.type_index());
    if (it != fs.end()) {
      it->second(value, attr);
      return true;
    } else {
      return false;
    }
  }

private:
  std::unordered_map<std::type_index, function> fs;
};

class value_processor
{
public:
  value_processor();

  void apply(const char *id, matador::value &val, const field_attributes &attr, serializer &s);

private:
  template < class T >
  void process(T &val, const field_attributes &attr, typename std::enable_if<std::is_integral<T>::value>::type* = 0)
  {
    serializer_->on_attribute(id_, val, attr);
  }

  template < class T >
  void process(T &val, const field_attributes &attr, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
  {
    serializer_->on_attribute(id_, val, attr);
  }

  void process(std::string &val, const field_attributes &attr);
  void process(char *val, const field_attributes &attr);
  void process(time &val, const field_attributes &attr);
  void process(date &val, const field_attributes &attr);

private:
  value_visitor visitor_;
  serializer *serializer_ = nullptr;
  const char *id_ = nullptr;
};

class value_to_string_processor
{
public:
  value_to_string_processor();

  std::string to_string(const matador::value &v);
  std::string to_safe_string(const matador::value &v, const basic_dialect *d);

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
#endif //MATADOR_VALUE_PROCESSOR_HPP
