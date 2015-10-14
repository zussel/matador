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
#include "database/result.hpp"

#include <string>
#include <functional>

namespace oos {

class serializable;
class sql;
class object_base_producer;
class database;

namespace detail {
  class statement_impl;
}

/// @cond OOS_DEV
template < class T >
class OOS_API statement
{
private:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  statement()
    : p(nullptr)
  { }

  statement(detail::statement_impl *impl, database *db)
    : p(impl)
    , db_(db)
  { }

  ~statement()
  {
    if (p) {
      p->clear();
      delete p;
    }
  }

  statement(statement &&x)
  {
    std::swap(p, x.p);
  }

  statement& operator=(statement &&x)
  {
    if (p) {
      delete p;
      p = nullptr;
    }
    std::swap(p, x.p);
    return *this;
  }

  void clear()
  {
    if (p) {
      p->clear();
    }
  }

  result<T> execute()
  {
//  std::cout << "SQL: " << p->str() << '\n';
    return result<T>(p->execute(), db_);
  }

  void reset()
  {
    p->reset();
  }
  
  int bind(T *o)
  {
    return p->bind(o);
  }

  template < class V >
  int bind(unsigned long i, const V &val)
  {
    return p->bind(i, val);
  }

  std::string str() const
  {
    return p->str();
  }

private:
  oos::detail::statement_impl *p = nullptr;
  database *db_ = nullptr;
};

/// @endcond

}

#endif /* STATEMENT_HPP */
