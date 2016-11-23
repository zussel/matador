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
    //std::cout << "SQL: " << p->str() << '\n';
    //std::cout.flush();
    return result<T>(p->execute());
  }

  void reset()
  {
    p->reset();
  }
  
  size_t bind(T *o, size_t pos)
  {
    return p->bind(o, pos);
  }

  template < class V >
  size_t bind(V &val, size_t pos)
  {
    return p->bind(val, pos);
  }

  std::string str() const
  {
    return p->str();
  }

private:
  oos::detail::statement_impl *p = nullptr;
};

template <>
class statement<row>
{
private:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  statement() {}
  statement(detail::statement_impl *impl, const row &prototype)
    : p(impl)
    , prototype_(prototype)
  { }

  ~statement()
  {
    if (p) {
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

  result<row> execute()
  {
    return result<row>(p->execute(), prototype_);
  }

  void reset()
  {
    p->reset();
  }

//  size_t bind(T *o, size_t pos)
//  {
//    return p->bind(o, pos);
//  }

  template < class V >
  size_t bind(V &val, size_t pos)
  {
    return p->bind(val, pos);
  }

  std::string str() const
  {
    return p->str();
  }

private:
  oos::detail::statement_impl *p = nullptr;
  const row prototype_;
};


}

#endif /* STATEMENT_HPP */
