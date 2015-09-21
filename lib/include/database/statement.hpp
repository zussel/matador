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

#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/object_atomizer.hpp"

#include "database/statement_impl.hpp"

#include <string>
#include <functional>

namespace oos {

class result;
class serializable;
class sql;
class object_base_producer;

namespace detail {
  class statement_impl;
}

    /// @cond OOS_DEV
class OOS_API statement
{
private:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  statement();
  statement(detail::statement_impl *impl);
  ~statement();

  statement(statement &&x);
  statement& operator=(statement &&x);

  void clear();

  result execute();

  void reset();
  
  int bind(serializable *o);
  template < class T >
  int bind(unsigned long i, const T &val)
  {
    return p->bind(i, val);
  }

  std::string str() const;

private:
  oos::detail::statement_impl *p = nullptr;
};

/// @endcond

}

#endif /* STATEMENT_HPP */
