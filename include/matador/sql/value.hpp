#ifndef VALUE_HPP
#define VALUE_HPP

#include "matador/sql/export.hpp"

#include "matador/sql/token.hpp"
#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/value_visitor.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/any.hpp"

#include <string>
#include <typeinfo>
#include <cstring>
#include <type_traits>

namespace matador {

/// @cond MATADOR_DEV

struct OOS_SQL_API value : public detail::token
{
  template<typename T, typename U = std::decay<T>>
  value(T &&val)
    : token(detail::token::VALUE)
    , value_(val)
  {}

  template<typename T, typename U = std::decay<T>>
  value(T &&val, std::size_t s)
    : token(detail::token::VALUE)
    , value_(val)
    , size_(s)
  {}

  template < class T > T get() {
    return value_._<T>();
  }

  void serialize(const char *id, serializer &srlzr);

  void accept(token_visitor &visitor) override;

  std::string str() const;
  std::string safe_string(const basic_dialect &) const;

  virtual const char* type_id() const;

  any value_;
  std::size_t size_ = 0;
  detail::value_visitor value_visitor_;
  detail::value_to_string_visitor value_to_string_visitor_;
};

//}

struct null_value : public value
{
  static std::string NULLSTR;

  null_value() : value(NULLSTR) { }

  const char* type_id() const override;
};

//template<class T>
//struct value<T, typename std::enable_if<
//  std::is_integral<T>::value &&
//  !std::is_same<char, T>::value &&
//  !std::is_same<char*, T>::value>::type> : public detail::basic_value
//{
//  explicit value(const T &val)
//    : basic_value(detail::token::VALUE)
//    , val(val)
//  { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << val;
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &) const override
//  {
//    std::stringstream str;
//    str << val;
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(T).name();
//  }
//
//  T val = {};
//};
//
//template < class T >
//struct value<T, typename std::enable_if<
//  std::is_floating_point<T>::value
//>::type > : public detail::basic_value
//{
//  explicit value(const T &val)
//  : basic_value(detail::token::VALUE)
//  , val(val)
//  { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << val;
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &) const override
//  {
//    std::stringstream str;
//    str << val;
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(T).name();
//  }
//
//  T val = {};
//};
//
//template<>
//struct value<std::string> : public detail::basic_value
//{
//  explicit value(std::string val)
//    : basic_value(detail::token::VALUE)
//    , val(std::move(val)) { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    return "'" + val + "'";
//  }
//
//  std::string safe_string(const basic_dialect &dialect) const override
//  {
//    return "'" + dialect.prepare_literal(val) + "'";
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(std::string).name();
//  }
//  std::string val;
//};
//
//template<>
//struct value<char> : public detail::basic_value
//{
//  explicit value(char val)
//    : basic_value(detail::token::VALUE)
//    , val(val) { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &) const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(char).name();
//  }
//  char val;
//};
//
//template<>
//struct value<unsigned char> : public detail::basic_value
//{
//  explicit value(unsigned char val)
//    : basic_value(detail::token::VALUE)
//    , val(val) { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &) const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(unsigned char).name();
//  }
//  unsigned char val;
//};
//
//template<>
//struct value<char*> : public detail::basic_value
//{
//  value(const char *v, size_t)
//    : basic_value(detail::token::VALUE)
//    , val(v)
//  {
//  }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &dialect) const override
//  {
//    std::stringstream str;
//    str << "'" <<  dialect.prepare_literal(val) << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(char*).name();
//  }
//
//  std::string val;
//};
//
//template<>
//struct value<const char*> : public detail::basic_value
//{
//  value(const char *v, size_t)
//    : basic_value(detail::token::VALUE)
//    , val(v)
//  {
//  }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << val << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &dialect) const override
//  {
//    std::stringstream str;
//    str << "'" << dialect.prepare_literal(val) << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(const char*).name();
//  }
//  std::string val;
//};
//
//template<>
//struct value<matador::date> : public detail::basic_value
//{
//  explicit value(matador::date val)
//    : basic_value(detail::token::VALUE)
//    , val(std::move(val)) { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << matador::to_string(val) << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &dialect) const override
//  {
//    std::stringstream str;
//    str << "'" << dialect.prepare_literal(matador::to_string(val)) << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(matador::date).name();
//  }
//  matador::date val;
//};
//
//template<>
//struct value<matador::time> : public detail::basic_value
//{
//  explicit value(matador::time val)
//    : basic_value(detail::token::VALUE)
//    , val(val)
//  { }
//
//  void serialize(const char *id, serializer &srlzr) override
//  {
//    srlzr.serialize(id, val);
//  }
//
//  std::string str() const override
//  {
//    std::stringstream str;
//    str << "'" << matador::to_string(val, "%FT%T.%f") << "'";
//    return str.str();
//  }
//
//  std::string safe_string(const basic_dialect &dialect) const override
//  {
//    std::stringstream str;
//    str << "'" << dialect.prepare_literal(matador::to_string(val, "%FT%T.%f")) << "'";
//    return str.str();
//  }
//
//  const char* type_id() const override
//  {
//    return typeid(matador::time).name();
//  }
//
//  matador::time val;
//};

template < class T >
value* make_value(const T &val)
{
  return new value(val);
}

value* make_value(const char* val, std::size_t len);

/// @endcond

}

#endif /* VALUE_HPP */
