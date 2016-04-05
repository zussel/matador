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

#include "sql/statement_impl.hpp"
#include "sql/result.hpp"

#include <string>
#include <functional>

namespace oos {

class sql;

namespace detail {
  class statement_impl;
}

/// @cond OOS_DEV
template < class T >
class statement
{
private:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  statement()
    : p(nullptr)
  { }

  statement(detail::statement_impl *impl)
    : p(impl)
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
    return result<T>(p->execute());
  }

  void reset()
  {
    p->reset();
  }
  
  size_t bind(T *o)
  {
    return p->bind(o);
  }

  template < class V >
  size_t bind(unsigned long i, V &val)
  {
    return p->bind(i, val);
  }

  std::string str() const
  {
    return p->str();
  }

private:
  oos::detail::statement_impl *p = nullptr;
};

/// @endcond

}

#endif /* STATEMENT_HPP */
