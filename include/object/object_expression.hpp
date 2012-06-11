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

#include "object/object_ptr.hpp"

#include <string>

namespace oos {

/// @cond OOS_DEV

template < class T >
class constant
{
public:
  constant(const T &c)
    : constant_(c)
  {}

  template < class O >
  T operator()(const object_ptr<O>&) const
  {
    return constant_;
  }

  template < class O >
  T operator()(const object_ref<O>&) const
  {
    return constant_;
  }

private:
  T constant_;
};

/// @endcond OOS_DEV
class null_var {};

template < class R, class O, class V = null_var >
class variable
{
public:
  typedef V var_type;
  typedef typename V::object_type super_type;
  typedef O object_type;
  typedef R return_type;
  typedef return_type (object_type::*memfunc_type)() const;

  variable(memfunc_type m, const var_type &v)
    : v_(v)
    , m_(m)
  {}

  return_type operator()(const object_ptr<super_type> &o) const
  {
    return (v_(o).get()->*m_)();
  }

  return_type operator()(const object_ref<super_type> &o) const
  {
    return (v_(o).get()->*m_)();
  }

private:
  var_type v_;
  memfunc_type m_;
};

/**
 * @class variable
 * @tparam T Type of the variable.
 * @tparam O Object class of the variable
 * @brief Holds the member function of an object as a variable.
 *
 * This class holds a member function of an object
 * and lets it be used as a variable. Wit the operator()()
 * one can call the member function.
 */
template < class R, class O >
class variable<R, O, null_var>
{
public:
  typedef O object_type;
  typedef R return_type;
  typedef return_type (object_type::*memfun)() const; /**< Shortcut for the member function. */

  /**
   * Creates a variable with the given member function
   *
   * @param m The member function to call.
   */
  variable(memfun m)
    : m_(m)
  {}

  return_type operator()(const object_ptr<object_type> &o) const
  {
    return (*o.get().*m_)();
  }

  return_type operator()(const object_ref<object_type> &o) const
  {
    return (*o.get().*m_)();
  }

private:
  memfun m_;
};

/// @cond OOS_DEV

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
struct expression_traits<bool>
{
  typedef constant<bool> expression_type;
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

/*
template < class O >
struct expression_traits<const object_base_ptr&>
{
  typedef constant<const object_base_ptr&> expression_type;
};
*/

template < class L, class OP, class O >
class unary_expression
{
public:
  typedef O object_type;

  unary_expression(const L &l, OP op = OP())
    : left_(l)
    , op_(op)
  {}


  bool operator()(const object_ptr<O> &o) const
  {
    return op_(left_(o));
  }

  bool operator()(const object_ref<O> &o) const
  {
    return op_(left_(o));
  }

private:
  typename expression_traits<L>::expression_type left_;
  OP op_;
};

template < class L, class R, class OP, class O >
class binary_expression
{
public:
  typedef O object_type;

  binary_expression(const L &l, const R &r, OP op = OP())
    : left_(l)
    , right_(r)
    , op_(op)
  {}

  bool operator()(const object_ptr<O> &o) const
  {
    return op_(left_(o), right_(o));
  }

  bool operator()(const object_ref<O> &o) const
  {
    return op_(left_(o), right_(o));
  }

private:
  typename expression_traits<L>::expression_type left_;
  typename expression_traits<R>::expression_type right_;
  OP op_;
};

// greater

template < class T, class O >
binary_expression<variable<T, O>, T, std::greater<T>, O > operator>(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::greater<T>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::greater<T>, O > operator>(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::greater<T>, O >(l, r);
}

// greater equal

template < class T, class O >
binary_expression<variable<T, O>, T, std::greater_equal<T>, O > operator>=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::greater_equal<T>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::greater_equal<T>, O > operator>=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::greater_equal<T>, O >(l, r);
}

// less

template < class T, class O >
binary_expression<variable<T, O>, T, std::less<T>, O > operator<(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::less<T>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::less<T>, O > operator<(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::less<T>, O >(l, r);
}

// less equal

template < class T, class O >
binary_expression<variable<T, O>, T, std::less_equal<T>, O > operator<=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::less_equal<T>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::less_equal<T>, O > operator<=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::less_equal<T>, O >(l, r);
}

// equal

template < class T, class O >
binary_expression<variable<T, O>, T, std::equal_to<T>, O > operator==(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::equal_to<T>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::equal_to<T>, O > operator==(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::equal_to<T>, O >(l, r);
}

template < class T, class O, class V >
binary_expression<variable<T, O, V>, T, std::equal_to<T>, typename V::object_type > operator==(const variable<T, O, V> &l, const T &r)
{
  return binary_expression<variable<T, O, V>, T, std::equal_to<T>, typename V::object_type >(l, r);
}

template < class T, class O, class V >
binary_expression<T, variable<T, O, V>, std::equal_to<T>, typename V::object_type > operator==(const T &l, const variable<T, O, V> &r)
{
  return binary_expression<T, variable<T, O, V>, std::equal_to<T>, typename V::object_type >(l, r);
}

template < class O >
binary_expression<variable<std::string, O>, const char*, std::equal_to<std::string>, O > operator==(const variable<std::string, O> &l, const char *r)
{
  return binary_expression<variable<std::string, O>, const char*, std::equal_to<std::string>, O >(l, r);
}

template < class O >
binary_expression<const char*, variable<std::string, O>, std::equal_to<std::string>, O > operator==(const char *t, const variable<std::string, O> &r)
{
  return binary_expression<const char*, variable<std::string, O>, std::equal_to<std::string>, O >(l, r);
}

// not equal

template < class T, class O >
binary_expression<variable<T, O>, T, std::not_equal_to<T>, O > operator!=(const variable<T, O> &l, const T &r)
{
  return binary_expression<variable<T, O>, T, std::not_equal_to<T>, O >(l, r);
}

template < class O >
binary_expression<variable<std::string, O>, const char*, std::not_equal_to<std::string>, O > operator!=(const variable<std::string, O> &l, const char *r)
{
  return binary_expression<variable<std::string, O>, const char*, std::not_equal_to<std::string>, O >(l, r);
}

template < class T, class O >
binary_expression<T, variable<T, O>, std::not_equal_to<T>, O > operator!=(const T &l, const variable<T, O> &r)
{
  return binary_expression<T, variable<T, O>, std::not_equal_to<T>, O >(l, r);
}

// logical

template < class L1, class R1, class OP1, class L2, class R2, class OP2, class O >
binary_expression<binary_expression<L1, R1, OP1, O>, 
                  binary_expression<L2, R2, OP2, O>,
                  std::logical_or<bool>, O > operator||(const binary_expression<L1, R1, OP1, O> &l,
                                                     const binary_expression<L2, R2, OP2, O> &r)
{
  return binary_expression<binary_expression<L1, R1, OP1, O>, binary_expression<L2, R2, OP2, O>, std::logical_or<bool>, O >(l, r);
}

template < class L1, class R1, class OP1, class L2, class R2, class OP2, class O >
binary_expression<binary_expression<L1, R1, OP1, O>, 
                  binary_expression<L2, R2, OP2, O>,
                  std::logical_and<bool>, O > operator&&(const binary_expression<L1, R1, OP1, O> &l,
                                                      const binary_expression<L2, R2, OP2, O> &r)
{
  return binary_expression<binary_expression<L1, R1, OP1, O>, binary_expression<L2, R2, OP2, O>, std::logical_and<bool>, O >(l, r);
}

template < class L, class R, class OP, class O >
unary_expression<binary_expression<L, R, OP, O>, std::logical_not<bool>, O > operator!(const binary_expression<L, R, OP, O> &l)
{
  return unary_expression<binary_expression<L, R, OP, O>, std::logical_not<bool>, O >(l);
}

/// @endcond

}

#endif /* OBJECT_EXPRESSION_HPP */
