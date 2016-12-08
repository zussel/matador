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

/**
 * The statement class represents a prepared
 * sql statement.
 * 
 * @tparam The object type of the statement to work on
 */
template < class T >
class statement
{
private:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  /**
   * Creates an empty statement
   */
  statement()
    : p(nullptr)
  { }

  /**
   * Creates a statement initialized from the
   * given statement implementation object holding
   * the implementation for the selected database
   *
   * @param impl The statement implementation object
   */
  statement(detail::statement_impl *impl)
    : p(impl)
  { }

  ~statement()
  {
    if (p) {
      delete p;
    }
  }

  /**
   * Copy move constructor for statement
   * 
   * @param x The statement to move from
   */
  statement(statement &&x)
  {
    std::swap(p, x.p);
  }

  /**
   * Assignment move constructor for statement
   * 
   * @param x The statement to move from
   */
  statement& operator=(statement &&x)
  {
    if (p) {
      delete p;
      p = nullptr;
    }
    std::swap(p, x.p);
    return *this;
  }

  /**
   * Clear the statement
   */
  void clear()
  {
    if (p) {
      p->clear();
    }
  }

  /**
   * Executes the prepared statement and returns
   * a result object. If the sql command was a
   * select the result object holds the queried
   * rows.
   * 
   * @return The result of the statement
   */
  result<T> execute()
  {
    //std::cout << "SQL: " << p->str() << '\n';
    //std::cout.flush();
    return result<T>(p->execute());
  }

  /**
   * Resets the statement by unbinding
   * all bindings.
   */
  void reset()
  {
    p->reset();
  }
  
  /**
   * Bind an object to the statement starting
   * at the given position index.
   * 
   * @param o The object to bind
   * @param pos The index where to start the binding
   * @return The next index to bind
   */
  size_t bind(T *o, size_t index)
  {
    return p->bind(o, index);
  }

  /**
   * Bind single value to a specified
   * position index of the prepared statement
   * 
   * @tparam The type of the value
   * @param val The value to bind
   * @param index The index where the value is to bind
   * @return The next index to bind
   */
  template < class V >
  size_t bind(V &val, size_t index)
  {
    return p->bind(val, index);
  }

  /**
   * Returns the statement as string where
   * the host values a shown as questionmarks (?)
   * 
   * @return The query string
   */
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
