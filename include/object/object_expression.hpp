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

#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"

#include <string>

namespace oos {

class object_base_ptr;


template < class T >
class constant
{
public:
  constant(const T &c)
    : constant_(c)
  {}

  T operator()(const object_base_ptr&) const
  {
    return constant_;
  }

private:
  T constant_;
};

template < class T, class O >
class variable
{
public:
  typedef T (O::*memfun)() const;
  variable(memfun m)
    : m_(m)
  {}
  T operator()(const object_base_ptr &optr) const
  {
    return get(optr.ptr());
  }

private:
  T get(const object *o) const
  {
    const O *obj = static_cast<const O*>(o);
    return (*obj.*m_)();
  }
private:
  memfun m_;
};

template < class E >
struct expression_traits
{
  typedef E expression_type;
};

template <>
struct expression_traits<int>
{
  typedef constant<int> expression_type;
};

template <>
struct expression_traits<double>
{
  typedef constant<double> expression_type;
};

template <>
struct expression_traits<std::string>
{
  typedef constant<std::string> expression_type;
};

template <>
struct expression_traits<varchar_base>
{
  typedef constant<varchar_base> expression_type;
};

template < class L, class R, class OP >
class binary_expression
{
public:
  binary_expression(const L &l, const R &r, OP op = OP())
    : left_(l)
    , right_(r)
    , op_(op)
  {}

  bool operator()(const object_base_ptr &o) const
  {
   return op_(left_(o), right_(o));
  }

private:
  typename expression_traits<L>::expression_type left_;
  typename expression_traits<R>::expression_type right_;
  OP op_;
};

// int shortcuts

template < class L, class R >
binary_expression<L, R, std::greater<int> > operator>(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater<int> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::less<R> > operator<(const L &l, const R &r)
{
  return binary_expression<L, R, std::less<int> >(l, r);
}

/*
template < class L, class R >
binary_expression<L, R, std::less_equal<int> > operator<=(const L &l, const R &r)
{
  return binary_expression<L, R, std::less_equal<int> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::equal_to<int> > operator==(const L &l, const R &r)
{
  return binary_expression<L, R, std::equal_to<int> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::not_equal_to<int> > operator!=(const L &l, const R &r)
{
  return binary_expression<L, R, std::not_equal_to<int> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::greater_equal<int> > operator>=(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater_equal<int> >(l, r);
}
*/
template < class L, class R >
binary_expression<L, R, std::logical_or<int> > operator||(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_or<int> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::logical_and<int> > operator&&(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_and<int> >(l, r);
}

// string shortcuts

template < class L, class R >
binary_expression<L, R, std::greater<std::string> > operator>(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater<std::string> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::less<std::string> > operator<(const L &l, const R &r)
{
  return binary_expression<L, R, std::less<std::string> >(l, r);
}
/*
template < class L, class R >
binary_expression<L, R, std::less_equal<std::string> > operator<=(const L &l, const R &r)
{
  return binary_expression<L, R, std::less_equal<std::string> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::equal_to<std::string> > operator==(const L &l, const R &r)
{
  return binary_expression<L, R, std::equal_to<std::string> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::not_equal_to<std::string> > operator!=(const L &l, const R &r)
{
  return binary_expression<L, R, std::not_equal_to<std::string> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::greater_equal<std::string> > operator>=(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater_equal<std::string> >(l, r);
}
*/
template < class L, class R >
binary_expression<L, R, std::logical_or<std::string> > operator||(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_or<std::string> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::logical_and<std::string> > operator&&(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_and<std::string> >(l, r);
}
/*
// varchar shortcuts

template < class L, class R >
binary_expression<L, R, std::greater<varchar_base> > operator>(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::less<varchar_base> > operator<(const L &l, const R &r)
{
  return binary_expression<L, R, std::less<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::less_equal<varchar_base> > operator<=(const L &l, const R &r)
{
  return binary_expression<L, R, std::less_equal<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::equal_to<varchar_base> > operator==(const L &l, const R &r)
{
  return binary_expression<L, R, std::equal_to<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::not_equal_to<varchar_base> > operator!=(const L &l, const R &r)
{
  return binary_expression<L, R, std::not_equal_to<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::greater_equal<varchar_base> > operator>=(const L &l, const R &r)
{
  return binary_expression<L, R, std::greater_equal<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::logical_or<varchar_base> > operator||(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_or<varchar_base> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::logical_and<varchar_base> > operator&&(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_and<varchar_base> >(l, r);
}
*/
}

#endif /* OBJECT_EXPRESSION_HPP */
