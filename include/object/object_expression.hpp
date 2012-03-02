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

#ifndef OBJECT_EXPRESSION_HPP
#define OBJECT_EXPRESSION_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include <string>

namespace oos {

class object;

template < class T >
class operand
{
public:
  virtual ~operand() {}

  virtual T value(object *o) const = 0;
};

template < class T >
class constant : public operand<T>
{
public:
  constant(const T &c)
    : constant_(c)
  {}
  constant& operator=(const T &c)
  {
    constant_ = c;
    return *this;
  }

  virtual ~constant() {}

  virtual T value(object *o) const
  {
    return constant_;
  }

private:
  const T constant_;
};

template < class T, class O >
class variable : public operand<T>
{
public:
  typedef T (O::*memfun)() const;
  variable(memfun m)
    : m_(m)
  {}
  virtual ~variable() {}
  
  virtual T value(object *o) const
  {
  }

private:
  T get(object *o) const
  {
    O *obj = static_cast<O*>(o);
    return (*obj.*m_)();
  }
private:
  memfun m_;
};

class expression
{
public:
  virtual ~expression() {}

  virtual bool eval(object *o) const = 0;
};

template < class T >
class unary_expression : public expression
{
public:
  unary_expression()
  {}
  virtual ~unary_expression() {}

  virtual bool eval(object *o) const
  {
    return true;
  }
};

template < class T, class OP >
class binary_expression : public expression
{
public:
  binary_expression(const operand<T> &l, const operand<T> &r)
    : left_(l)
    , right_(r)
  {}
  virtual ~binary_expression() {}

  virtual bool eval(object *o) const
  {
    return OP(left_, right_);
  }

private:
  operand<T> left_;
  operand<T> right_;
};

template < class T >
expression* operator<(const operand<T> &l, const operand<T> &r)
{
  return new binary_expression<T, std::less<T> >(l, r);
}

}

#endif /* OBJECT_EXPRESSION_HPP */