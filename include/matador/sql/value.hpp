/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALUE_HPP
#define VALUE_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/token.hpp"
#include "matador/sql/basic_dialect.hpp"

#include "matador/utils/varchar.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/serializer.hpp"

#include <string>
#include <typeinfo>
#include <cstring>

namespace matador {

template<class T, class Enabled = void>
struct value;

namespace detail {
/// @cond MATADOR_DEV

struct OOS_SQL_API basic_value : public token
{
  explicit basic_value(token::t_token tok) : token(tok) { }

  template < class T > T get() {
    auto *v = dynamic_cast<matador::value<T>* >(this);
    if (v) {
      return v->val;
    } else {
      throw std::bad_cast();
    }
  }

  virtual void serialize(const char *id, serializer &srlzr) = 0;

  void accept(token_visitor &visitor) override;

  virtual std::string str() const = 0;
  virtual std::string safe_string(const basic_dialect &) const = 0;

  virtual const char* type_id() const = 0;
};

}

struct null_value : public detail::basic_value
{
  static std::string NULLSTR;

  null_value() : basic_value(detail::token::VALUE) { }

  void serialize(const char *id, serializer &srlzr) override;

  std::string str() const override;
  std::string safe_string(const basic_dialect &) const override;

  const char* type_id() const override;
};

template<class T>
struct value<T, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<char, T>::value &&
  !std::is_same<char*, T>::value>::type> : public detail::basic_value
{
  explicit value(const T &val)
    : basic_value(detail::token::VALUE)
    , val(val)
  { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << val;
    return str.str();
  }

  std::string safe_string(const basic_dialect &) const override
  {
    std::stringstream str;
    str << val;
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(T).name();
  }

  T val;
};


template<class T>
struct value<T, typename std::enable_if<std::is_base_of<matador::varchar_base, T>::value>::type> : public detail::basic_value
{
  explicit value(const T &val)
    : basic_value(detail::token::VALUE)
    , val(val) { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    return "'" + val.str() + "'";
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    return "'" + dialect.prepare_literal(val.str()) + "'";
  }

  const char* type_id() const override
  {
    return typeid(T).name();
  }
  T val;
};

template<>
struct value<std::string> : public detail::basic_value
{
  explicit value(std::string val)
    : basic_value(detail::token::VALUE)
    , val(std::move(val)) { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    return "'" + val + "'";
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    return "'" + dialect.prepare_literal(val) + "'";
  }

  const char* type_id() const override
  {
    return typeid(std::string).name();
  }
  std::string val;
};

template<>
struct value<char> : public detail::basic_value
{
  explicit value(char val)
    : basic_value(detail::token::VALUE)
    , val(val) { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  std::string safe_string(const basic_dialect &) const override
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(char).name();
  }
  char val;
};

template<>
struct value<char*> : public detail::basic_value
{
  value(const char *v, size_t)
    : basic_value(detail::token::VALUE)
    , val(v)
  {
  }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << "'" <<  dialect.prepare_literal(val) << "'";
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(char*).name();
  }

  std::string val;
};

template<>
struct value<const char*> : public detail::basic_value
{
  value(const char *v, size_t)
    : basic_value(detail::token::VALUE)
    , val(v)
  {
  }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << "'" << dialect.prepare_literal(val) << "'";
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(const char*).name();
  }
  std::string val;
};

template<>
struct value<matador::date> : public detail::basic_value
{
  explicit value(matador::date val)
    : basic_value(detail::token::VALUE)
    , val(std::move(val)) { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << "'" << matador::to_string(val) << "'";
    return str.str();
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << "'" << dialect.prepare_literal(matador::to_string(val)) << "'";
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(matador::date).name();
  }
  matador::date val;
};

template<>
struct value<matador::time> : public detail::basic_value
{
  explicit value(matador::time val)
    : basic_value(detail::token::VALUE)
    , val(std::move(val))
  { }

  void serialize(const char *id, serializer &srlzr) override
  {
    srlzr.serialize(id, val);
  }

  std::string str() const override
  {
    std::stringstream str;
    str << "'" << matador::to_string(val, "%FT%T.%f") << "'";
    return str.str();
  }

  std::string safe_string(const basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << "'" << dialect.prepare_literal(matador::to_string(val, "%FT%T.%f")) << "'";
    return str.str();
  }

  const char* type_id() const override
  {
    return typeid(matador::time).name();
  }

  matador::time val;
};

template < class T >
detail::basic_value* make_value(const T &val)
{
  return new value<T>(val);
}

detail::basic_value* make_value(const char* val, size_t len);

/// @endcond

}

#endif /* VALUE_HPP */
