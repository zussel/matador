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

#include "sql/token.hpp"

#include <string>
#include <typeinfo>

namespace oos {

template<class T, class Enabled = void>
struct value;

namespace detail {
/// @cond OOS_DEV

struct basic_value : public token
{
  basic_value(basic_dialect::t_token tok) : token(tok) { }

  template < class T > T get() const {
    oos::value<T> *v = dynamic_cast<oos::value<T>* >(this);
    if (v) {
      return v->template get<T>();
    } else {
      throw std::bad_cast();
    }
  }

  std::string value() const
  {
    return str();
  }

  virtual std::string compile(basic_dialect &d) const;

  virtual std::string str() const = 0;
};

}

template<class T>
struct value<T, typename std::enable_if<std::is_scalar<T>::value>::type> : public detail::basic_value
{
  value(T val)
    : basic_value(basic_dialect::VALUE)
//    , column(col)
//    , type(t)
    , val(val) { }

  std::string str() const
  {
    std::stringstream str;
    str << val;
    return str.str();
  }

  std::string column;
  data_type_t type;
  T val;
};

template<class T>
struct value<T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char *, T>::value>::type> : public detail::basic_value
{
  value(T val)
    : basic_value(basic_dialect::VALUE)
//    , column(col)
//    , type(t)
    , val(val) { }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  std::string column;
  data_type_t type;
  T val;
};

/// @endcond

}

#endif /* VALUE_HPP */
