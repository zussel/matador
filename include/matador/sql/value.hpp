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

struct null_value : public value
{
  static std::string NULLSTR;

  null_value() : value(NULLSTR) { }

  const char* type_id() const override;
};

template < class T >
value* make_value(const T &val)
{
  return new value(val);
}

value* make_value(const char* val, std::size_t len);

/// @endcond

}

#endif /* VALUE_HPP */
