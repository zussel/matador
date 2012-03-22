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

//#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"

#include <string>

namespace oos {

/// @cond OOS_DEV

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
struct expression_traits<const char*>
{
  typedef constant<std::string> expression_type;
};

template <>
struct expression_traits<const object_base_ptr&>
{
  typedef constant<const object_base_ptr&> expression_type;
};

template < class L, class OP >
class unary_expression
{
public:
  unary_expression(const L &l, OP op = OP())
    : left_(l)
    , op_(op)
  {}

  bool operator()(const object_base_ptr &o) const
  {
    return op_(left_(o));
  }

private:
  typename expression_traits<L>::expression_type left_;
  OP op_;
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

// greater

template < class T >
binary_expression<T, T, std::greater<T> > operator>(const T &l, const T &r)
{
  return binary_expression<T, T, std::greater<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::greater<T> > operator>(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::greater<T> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::greater<T> > operator>(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::greater<T> >(l, r);
}

// greater equal

template < class T >
binary_expression<T, T, std::greater_equal<T> > operator>=(const T &l, const T &r)
{
  return binary_expression<T, T, std::greater_equal<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::greater_equal<T> > operator>=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::greater_equal<T> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::greater_equal<T> > operator>=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::greater_equal<T> >(l, r);
}

// less

template < class T >
binary_expression<T, T, std::less<T> > operator<(const T &l, const T &r)
{
  return binary_expression<T, T, std::less<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::less<T> > operator<(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::less<T> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::less<T> > operator<(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::less<T> >(l, r);
}

// less equal

template < class T >
binary_expression<T, T, std::less_equal<T> > operator<=(const T &l, const T &r)
{
  return binary_expression<T, T, std::less_equal<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::less_equal<T> > operator<=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::less_equal<T> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::less_equal<T> > operator<=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::less_equal<T> >(l, r);
}

// equal

template < class T >
binary_expression<T, T, std::equal_to<T> > operator==(const T &l, const T &r)
{
  return binary_expression<T, T, std::equal_to<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::equal_to<T> > operator==(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::equal_to<T> >(l, r);
}

template < class O >
binary_expression<variable<std::string, O>, const char*, std::equal_to<std::string> > operator==(const variable<std::string, O> &l, const char *r)
{
  return binary_expression<variable<std::string, O>, const char*, std::equal_to<std::string> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::equal_to<T> > operator==(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::equal_to<T> >(l, r);
}

// not equal

template < class T >
binary_expression<T, T, std::not_equal_to<T> > operator!=(const constant<T> &l, const constant<T> &r)
{
  return binary_expression<T, T, std::not_equal_to<T> >(l, r);
}

template < class T, class O >
binary_expression<variable<T, O>, T, std::not_equal_to<T> > operator!=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::not_equal_to<T> >(l, r);
}

template < class O >
binary_expression<variable<std::string, O>, const char*, std::not_equal_to<std::string> > operator!=(const variable<std::string, O> &l, const char *r)
{
  return binary_expression<variable<std::string, O>, const char*, std::not_equal_to<std::string> >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::not_equal_to<T> > operator!=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::not_equal_to<T> >(l, r);
}

// logical

template < class L, class R >
binary_expression<L, R, std::logical_or<bool> > operator||(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_or<bool> >(l, r);
}

template < class L, class R >
binary_expression<L, R, std::logical_and<bool> > operator&&(const L &l, const R &r)
{
  return binary_expression<L, R, std::logical_and<bool> >(l, r);
}
/*
template < class L >
unary_expression<L, std::logical_not<bool> > operator!(const L &l)
{
  return unary_expression<L, std::logical_not<bool> >(l);
}
*/

/// @endcond

}

#endif /* OBJECT_EXPRESSION_HPP */
