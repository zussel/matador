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

#ifdef _MSC_VER
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

  T operator()(const object_base_ptr&) const
  {
    return constant_;
  }

private:
  T constant_;
};

template < class R >
class variable_impl
{
public:
  typedef R return_type;

  virtual ~variable_impl() {}
  
  virtual return_type operator()(const object_base_ptr &optr) const = 0;
};

template < class R, class O, class V >
class object_variable_impl : public variable_impl<R>
{
public:
  typedef V var_type;
  typedef O object_type;
  typedef R return_type;
  typedef return_type (object_type::*memfunc_type)() const;

  object_variable_impl(memfunc_type m, const var_type &v)
    : v_(v)
    , m_(m)
  {}
  virtual ~object_variable_impl() {}

  virtual return_type operator()(const object_base_ptr &optr) const
  {
    return (static_cast<const object_type*>(v_(optr).ptr())->*m_)();
  }

private:
  var_type v_;
  memfunc_type m_;
};

class null_var {};

template < class R, class O >
class object_variable_impl<R, O, null_var> : public variable_impl<R>
{
public:
  typedef O object_type;
  typedef R return_type;
  typedef return_type (object_type::*memfunc_type)() const;

  object_variable_impl(memfunc_type m)
    : m_(m)
  {}
  virtual ~object_variable_impl() {}

  virtual return_type operator()(const object_base_ptr &optr) const
  {
    return (static_cast<const object_type*>(optr.ptr())->*m_)();
  }

private:
  memfunc_type m_;
};

/// @endcond OOS_DEV

/**
 * @tparam R Type of the variable
 * @class variable
 * @brief Holds the functor to a method
 * 
 * Wrapper class for the concrete variable
 * implementation.
 */
template < class R >
class variable
{
public:
  typedef R return_type; /**< Shortcut for return type. */

  /**
   * Initializes a variable with a
   * pointer to a concrete variable
   * implemnation.
   * 
   * @param impl The concrete variable.
   */
  explicit variable(variable_impl<R> *impl)
    : impl_(impl)
  {}

  /**
   * Copies from the given variable.
   * 
   * @param x The variable to copy from.
   */
  variable(const variable &x)
    : impl_(x.impl_)
  {}

  /**
   * Assigns from the given variable.
   * 
   * @param x The variable to assign from.
   * @return Initialized variable.
   */
  variable& operator=(const variable &x)
  {
    impl_ = x.impl_;
    return *this;
  }
  ~variable() {}

  /**
   * Applies the () operator of the concrete
   * variable and returns the result.
   * 
   * @param optr The object to apply the variable to.
   * @return The value of the variable.
   */
  return_type operator()(const object_base_ptr &optr) const
  {
    return impl_->operator()(optr);
  }
  
private:
  std::shared_ptr<variable_impl<R> > impl_;
};

/**
 * @file object_expression.hpp
 * @brief Contains functions to create variables
 *
 * This file contains some functions creating
 * variables. These functions wrap the constructor
 * of the variable class and ease the creation
 * of variables of cascading method calls.
 */
 
 /**
  * @tparam R The return value type
  * @tparam O The object type
  * @brief Create a variable with depth zero
  * 
  * Creates a variable with depth zero. That means that the
  * value is inside the object itself.
  * 
  * @param mem_func A member function of the object_type.
  * @return A variable with return type R.
  */
template < class R, class O >
variable<R>
make_var(R (O::*mem_func)() const)
{
  return variable<R>(new object_variable_impl<R, O, null_var>(mem_func));
}

 /**
  * @tparam R The return value type
  * @tparam O The proxy object type
  * @tparam O1 The object type
  * @brief Create a variable with depth one
  * 
  * Creates a variable with depth one. That means that a
  * value of a nested object is requested.
  * 
  * @param mem_func A member function of the object_type.
  * @param mem_func_1 A member function of the nested object_type.
  * @return A variable with return type R.
  */
template < class R, class O, class O1 >
variable<R>
make_var(O1 (O::*mem_func)() const, R (O1::object_type::*mem_func_1)() const)
{
  return variable<R>(new object_variable_impl<R, typename O1::object_type, variable<O1> >(mem_func_1, make_var(mem_func)));
}

 /**
  * @tparam R The return value type
  * @tparam O The proxy object type
  * @tparam O1 The nested object type
  * @tparam O2 The nested object type
  * @brief Create a variable with depth two
  * 
  * Creates a variable with depth one. That means that a
  * value of a nested object is requested.
  * 
  * @param mem_func A member function of the object_type.
  * @param mem_func_1 A member function of the nested object_type.
  * @param mem_func_2 A member function of the nested object_type.
  * @return A variable with return type R.
  */
template < class R, class O, class O1, class O2 >
variable<R>
make_var(O1 (O::*mem_func)() const, O2 (O1::object_type::*mem_func_1)() const, R (O2::object_type::*mem_func_2)() const)
{
  return variable<R>(new object_variable_impl<R, typename O2::object_type, variable<O2> >(mem_func_2, make_var(mem_func, mem_func_1)));
}

 /**
  * @tparam R The return value type
  * @tparam O The proxy object type
  * @tparam O1 The nested object type
  * @tparam O2 The nested object type
  * @tparam O3 The nested object type
  * @brief Create a variable with depth two
  * 
  * Creates a variable with depth one. That means that a
  * value of a nested object is requested.
  * 
  * @param mem_func A member function of the object_type.
  * @param mem_func_1 A member function of the nested object_type.
  * @param mem_func_2 A member function of the nested object_type.
  * @param mem_func_3 A member function of the nested object_type.
  * @return A variable with return type R.
  */
template < class R, class O, class O1, class O2, class O3 >
variable<R>
make_var(O1 (O::*mem_func)() const, O2 (O1::object_type::*mem_func_1)() const, O3 (O2::object_type::*mem_func_2)() const, R (O3::object_type::*mem_func_3)() const)
{
  return variable<R>(new object_variable_impl<R, typename O3::object_type, variable<O3> >(mem_func_3, make_var(mem_func, mem_func_1, mem_func_2)));
}

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

template < class T >
struct expression_traits<object_ptr<T> >
{
  typedef constant<object_ptr<T> > expression_type;
};

template < class T >
struct expression_traits<object_ref<T> >
{
  typedef constant<object_ref<T> > expression_type;
};

template < class L, class OP >
class unary_expression
{
public:
  unary_expression(const L &l, OP op = OP())
    : left_(l)
    , op_(op)
  {}


  bool operator()(const object_base_ptr &optr) const
  {
    return op_(left_(optr));
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

  bool operator()(const object_base_ptr &optr) const
  {
    return op_(left_(optr), right_(optr));
  }

private:
  typename expression_traits<L>::expression_type left_;
  typename expression_traits<R>::expression_type right_;
  OP op_;
};

/**
 * this implements the greater
 * specialization for the binary
 * expression:
 *
 * variable > T
 * T > variable
 * variable<V> > T
 * T > variable<V>
 * variable<string> > const char*
 * const char* > variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::greater<T> > operator>(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::greater<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::greater<T> > operator>(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::greater<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::greater<std::string> > operator>(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::greater<std::string> >(l, r);
}

binary_expression<const char*, variable<std::string>, std::greater<std::string> > operator>(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::greater<std::string> >(l, r);
}
*/
/**
 * this implements the greater equal
 * specialization for the binary
 * expression:
 *
 * variable >= T
 * T >= variable
 * variable<V> >= T
 * T >= variable<V>
 * variable<string> >= const char*
 * const char* >= variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::greater_equal<T> > operator>=(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::greater_equal<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::greater_equal<T> > operator>=(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::greater_equal<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::greater_equal<std::string> > operator>=(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::greater_equal<std::string> >(l, r);
}

binary_expression<const char*, variable<std::string>, std::greater_equal<std::string> > operator>=(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::greater_equal<std::string> >(l, r);
}
*/
/**
 * this implements the less
 * specialization for the binary
 * expression:
 *
 * variable < T
 * T < variable
 * variable<V> < T
 * T < variable<V>
 * variable<string> < const char*
 * const char* < variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::less<T> > operator<(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::less<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::less<T> > operator<(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::less<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::less<std::string> > operator<(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::less<std::string> >(l, r);
}

binary_expression<const char*, variable<std::string>, std::less<std::string> > operator<(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::less<std::string> >(l, r);
}
*/
/**
 * this implements the less equal
 * specialization for the binary
 * expression:
 *
 * variable <= T
 * T <= variable
 * variable<V> <= T
 * T <= variable<V>
 * variable<string> <= const char*
 * const char* <= variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::less_equal<T> > operator<=(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::less_equal<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::less_equal<T> > operator<=(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::less_equal<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::less_equal<std::string> > operator<=(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::less_equal<std::string> >(l, r);
}

binary_expression<const char*, variable<std::string>, std::less_equal<std::string> > operator<=(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::less_equal<std::string> >(l, r);
}
*/
/**
 * this implements the equal
 * specialization for the binary
 * expression:
 *
 * variable == T
 * T == variable
 * variable<V> == T
 * T == variable<V>
 * variable<string> == const char*
 * const char* == variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::equal_to<T> > operator==(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::equal_to<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::equal_to<T> > operator==(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::equal_to<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::equal_to<std::string> > operator==(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::equal_to<std::string> >(l, r);
}


binary_expression<const char*, variable<std::string>, std::equal_to<std::string> > operator==(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::equal_to<std::string> >(l, r);
}
*/
/**
 * this implements the not equal
 * specialization for the binary
 * expression:
 *
 * variable != T
 * T != variable
 * variable<V> != T
 * T != variable<V>
 * variable<string> != const char*
 * const char* != variable<string>
 */
template < class T >
binary_expression<variable<T>, T, std::not_equal_to<T> > operator!=(const variable<T> &l, const T &r)
{
  return binary_expression<variable<T>, T, std::not_equal_to<T> >(l, r);
}

template < class T >
binary_expression<T, variable<T>, std::not_equal_to<T> > operator!=(const T &l, const variable<T> &r)
{
  return binary_expression<T, variable<T>, std::not_equal_to<T> >(l, r);
}
/*
binary_expression<variable<std::string>, const char*, std::not_equal_to<std::string> > operator!=(const variable<std::string> &l, const char *r)
{
  return binary_expression<variable<std::string>, const char*, std::not_equal_to<std::string> >(l, r);
}

binary_expression<const char*, variable<std::string>, std::not_equal_to<std::string> > operator!=(const char *l, const variable<std::string> &r)
{
  return binary_expression<const char*, variable<std::string>, std::not_equal_to<std::string> >(l, r);
}
*/
// logical

template < class L1, class R1, class OP1, class L2, class R2, class OP2 >
binary_expression<binary_expression<L1, R1, OP1>, 
                  binary_expression<L2, R2, OP2>,
                  std::logical_or<bool> > operator||(const binary_expression<L1, R1, OP1> &l,
                                                     const binary_expression<L2, R2, OP2> &r)
{
  return binary_expression<binary_expression<L1, R1, OP1>, binary_expression<L2, R2, OP2>, std::logical_or<bool> >(l, r);
}

template < class L1, class R1, class OP1, class L2, class R2, class OP2 >
binary_expression<binary_expression<L1, R1, OP1>, 
                  binary_expression<L2, R2, OP2>,
                  std::logical_and<bool> > operator&&(const binary_expression<L1, R1, OP1> &l,
                                                      const binary_expression<L2, R2, OP2> &r)
{
  return binary_expression<binary_expression<L1, R1, OP1>, binary_expression<L2, R2, OP2>, std::logical_and<bool> >(l, r);
}

template < class L, class R, class OP >
unary_expression<binary_expression<L, R, OP>, std::logical_not<bool> > operator!(const binary_expression<L, R, OP> &l)
{
  return unary_expression<binary_expression<L, R, OP>, std::logical_not<bool> >(l);
}

/// @endcond

}

#endif /* OBJECT_EXPRESSION_HPP */
