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
#ifdef oos_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "oos/sql/token.hpp"

#include "oos/utils/varchar.hpp"
#include "oos/utils/date.hpp"
#include "oos/utils/time.hpp"
#include "oos/utils/string.hpp"
#include "oos/utils/serializer.hpp"

#include <string>
#include <typeinfo>
#include <cstring>

namespace oos {

template<class T, class Enabled = void>
struct value;

namespace detail {
/// @cond OOS_DEV

struct OOS_SQL_API basic_value : public token
{
  basic_value(token::t_token tok) : token(tok) { }

  template < class T > T get() {
    oos::value<T> *v = dynamic_cast<oos::value<T>* >(this);
    if (v) {
      return v->val;
    } else {
      std::cout << "bad value cast: from " << type_id() << " to " << typeid(T).name() << "\n";
      throw std::bad_cast();
    }
  }

  std::string value() const
  {
    return str();
  }

  virtual void serialize(const char *id, serializer &srlzr) = 0;

  virtual void accept(token_visitor &visitor);

  virtual std::string str() const = 0;

  virtual const char* type_id() const = 0;
};

}

struct null_value : public detail::basic_value
{
  static std::string NULLSTR;

  null_value() : basic_value(detail::token::VALUE) { }

  virtual void serialize(const char *id, serializer &srlzr);

  std::string str() const;

  const char* type_id() const;
};

template<class T>
struct value<T, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<char, T>::value &&
  !std::is_same<char*, T>::value>::type> : public detail::basic_value
{
  value(const T &val)
    : basic_value(detail::token::VALUE)
    , val(val)
  { }

  virtual void serialize(const char *id, serializer &srlzr)
  {
    srlzr.serialize(id, val);
  }

  std::string str() const
  {
    std::stringstream str;
    str << val;
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(T).name();
  }

  T val;
};


template<class T>
struct value<T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_base_of<oos::varchar_base, T>::value>::type> : public detail::basic_value
{
  value(const T &val)
    : basic_value(detail::token::VALUE)
    , val(val) { }

  virtual void serialize(const char *id, serializer &srlzr)
  {
    srlzr.serialize(id, val);
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(T).name();
  }
  T val;
};

template<>
struct value<char> : public detail::basic_value
{
  value(char val)
    : basic_value(detail::token::VALUE)
    , val(val) { }

  virtual void serialize(const char *id, serializer &srlzr)
  {
    srlzr.serialize(id, val);
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(char).name();
  }
  char val;
};

template<>
struct value<char*> : public detail::basic_value
{
  value(const char *v, size_t l)
    : basic_value(detail::token::VALUE)
    , val(v, v+l)
  {
    val.push_back('\0');
  }

  virtual void serialize(const char *id, serializer &srlzr)
  {

    char *begin = &val.front();
    srlzr.serialize(id, begin, val.size());
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << &val.front() << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(char*).name();
  }

  std::vector<char> val;
};

template<>
struct value<const char*> : public detail::basic_value
{
  value(const char *v, size_t l)
    : basic_value(detail::token::VALUE)
    , val(v, v+l)
  {
    val.push_back('\0');
  }

  virtual void serialize(const char *id, serializer &srlzr)
  {

    char *begin = &val.front();
    srlzr.serialize(id, begin, val.size());
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << &val.front() << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(const char*).name();
  }
  std::vector<char> val;
};

template<>
struct value<oos::date> : public detail::basic_value
{
  value(const oos::date &val)
    : basic_value(detail::token::VALUE)
    , val(val) { }

  virtual void serialize(const char *id, serializer &srlzr)
  {
    srlzr.serialize(id, val);
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << oos::to_string(val) << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(oos::date).name();
  }
  oos::date val;
};

template<>
struct value<oos::time> : public detail::basic_value
{
  value(const oos::time &val)
    : basic_value(detail::token::VALUE)
    , val(val)
  { }

  virtual void serialize(const char *id, serializer &srlzr)
  {
    srlzr.serialize(id, val);
  }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << oos::to_string(val, "%FT%T.%f") << "'";
    return str.str();
  }

  const char* type_id() const
  {
    return typeid(oos::time).name();
  }

  oos::time val;
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
