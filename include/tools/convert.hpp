/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it wicll be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONVERT_HPP
#define CONVERT_HPP

#ifdef WIN32
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

#include "tools/varchar.hpp"
#include "tools/enable_if.hpp"

#include <type_traits>
#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <limits>
#include <cstring>

/// @cond OOS_DEV

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

/// @endcond

/**
 * @file convert.hpp
 * @brief Provides gemeric conversion functions.
 * 
 * This file provides generic conversion function.
 * All builtin types, std::string and oos::varchar_base
 * types are supported. User can select between three
 * conversion policies:
 * - strict: Input type must be output type.
 * - fitting: As long as the size of the types fits or it is a string.
 * - weak: Conversion is done although data loss could take place.
 *
 */

namespace oos {

class varchar_base;

/**
 * Coversion policies. There are three
 * policies:
 * 
 * - strict: Input must be output type.
 * - fitting: As long as the size of the types fits or it is a string.
 * - weak: Conversion is done although data loss could take place.
 * 
 */
typedef enum {
  convert_strict         = 1 << 0, /**< Strict policy for conversion */
  convert_fitting        = 1 << 1, /**< Fitting policy for conversion */
  convert_weak           = 1 << 2, /**< Weak policy for conversion */
  convert_strict_fitting = convert_strict | convert_fitting,  /**< Shortcut for strict and fitting policy combination */
  convert_fitting_weak   = convert_fitting | convert_weak,  /**< Shortcut for fitting and weak policy combination */
  convert_all            = convert_fitting | convert_weak | convert_strict  /**< Shortcut for all policies combination */
} t_convert_policy;

#ifdef OOS_DOXYGEN_DOC

/**
 * Convert a paramater of one type into another. The conversion
 * strategy is selected by the given policy. the default policy is
 * <i>fitting</i>. If the strategy discovers that a conversion
 * is not allowed an exception is thrown.
 * 
 * @tparam P Concersion strategy policy.
 * @tparam T Type of from parameter.
 * @tparam U Type of to parameter.
 * @param from From value to convert.
 * @param to Result parameter.
 * @throw std::bad_cast() exception.
 */
template < int P, class T, class U >
void
convert(const T &from, U &to);

#else

/*
 * from
 *   T
 * to
 *   U
 * where T and U are not the same type
 */
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_strict && !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0)
{
  throw std::bad_cast();
}
/*
 * from
 *   T
 * to
 *   U
 * where T and U are the same type
 */
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_strict && CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0)
{
  to = from;
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value) ||
                                (CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    integral U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value) ||
                                (CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value) ||
                                (CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    signed
 * to
 *    unsigned
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)abs(from);
}
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  if (from < 0) {
    throw std::bad_cast();
  } else {
    to = (U)from;
  }
}
/*******************************************
 * 
 * Convert from
 *    signed
 * to
 *    unsigned
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CP == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)abs(from);
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_signed<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   less
 */
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((P & convert_fitting) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<(sizeof(T) < sizeof(U))>::type* = 0)
{
  to = from;
}
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((P & convert_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value)>::type* = 0)
{
  to = static_cast<U>(from);
}
/*
 * from
 *   unsigned
 * to
 *   signed
 * where
 *   greater equal
 */
template < int P, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<P == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<U>::value)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) < sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak || P == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
/*******************************************
 * 
 * Cannot convert from
 *    floating point T
 * to
 *    floating point U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
template < int P, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<P == convert_fitting/* || P == convert_strict*/>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
/*******************************************
 * 
 * Convert from
 *    integral T
 * to
 *    floating point U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value &&
                                CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    integral U
 * where
 *    size of T is less than size of U
 *
 *******************************************/
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0/*,
        typename oos::enable_if<(std::numeric_limits<T>::digits > std::numeric_limits<U>::digits)>::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0*/)
{
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<!(std::numeric_limits<T>::digits > std::numeric_limits<U>::digits)>::type* = 0/*,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0*/)
{
//  to = (U)from;
  throw std::bad_cast();
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(std::numeric_limits<T>::digits > std::numeric_limits<U>::digits)>::type* = 0/*,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0*/)
{
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    floating point T
 * to
 *    integral U
 * where
 *    size of T is greater equal than size of U
 *
 *******************************************/
 /*
template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<P == convert_weak>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  to = (U)from;
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CP == convert_fitting>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                                CPP11_TYPE_TRAITS_NS::is_integral<U>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value>::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  throw std::bad_cast();
}
*/
/*****************
 * from
 *   bool
 * to
 *   arithmetic
 *
 *****************/
template < int P, class T >
void
convert(bool from, T &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value>::type* = 0)
{
  to = (from ? 1 : 0);
}

template < int P, class T >
void
convert(bool from, T &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0)
{
  to = (T)(from ? 1.0 : 0.0);
}
/*****************
 * from
 *   arithmetic
 * to
 *   bool
 *
 *****************/
template < int P, class T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value>::type* = 0)
{
  to = from > 0;
}

/*****************
 * from
 *   bool
 * to
 *   bool
 *
 *****************/
template < int P >
void
convert(const bool &from, bool &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0)
{
  to = from;
}

template < int P, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((P & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value &&
                                !CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value &&
                                CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0)
{
  to = from;
}

/*
 * from
 *   bool
 * to
 *   char array
 */
template < int CP, class S >
void
convert(bool from, char *to, S size,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
#ifdef WIN32
  _snprintf_s(to, size, size, (from ? "true" : "false"));
#else
  snprintf(to, size, (from ? "true" : "false"));
#endif
}

template < int CP, class S >
void
convert(bool , char *, S ,
        typename oos::enable_if<CP == convert_strict>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   integral
 * to
 *   char array
 */
template < int CP, class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, char>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, unsigned char>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
#ifdef WIN32
  _snprintf_s(to, size, size, "%d", from);
#else
  snprintf(to, size, "%lu", (long unsigned int)from);
#endif
}
/*
 * from
 *   integral
 * to
 *   char array
 */
template < int CP, class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_same<T, char>::value ||
                                CPP11_TYPE_TRAITS_NS::is_same<T, unsigned char>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
#ifdef WIN32
  _snprintf_s(to, size, size, "%c", from);
#else
  snprintf(to, size, "%c", from);
#endif
}
template < int CP, class T, class S >
void
convert(const T &from, char *to, S size,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, char>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
#ifdef WIN32
  _snprintf_s(to, size, size, "%d", from);
#else
  snprintf(to, size, "%ld", (long unsigned int)from);
#endif
}
template < int CP, class T, class S >
void
convert(T , char *, S ,
        typename oos::enable_if<CP == convert_strict>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, char>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   floating point
 * to
 *   char array
 */
template < int CP, class T, class S, class P >
void
convert(const T &from, char *to, S size, P precision,
        typename oos::enable_if<((CP & convert_all) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<P>::value>::type* = 0)
{
  char buf[32];
#ifdef WIN32
  _snprintf_s(buf, 32, 32, "%%0.%df", precision);
  _snprintf_s(to, size, size, buf, from);
#else
  snprintf(buf, 32, "%%0.%df", precision);
  snprintf(to, size, buf, from);
#endif
}

/*
 * from
 *   T
 * to
 *   char array
 */
template < int CP, class T, class S >
void
convert(const T &, char *, S ,
        typename oos::enable_if<((CP & convert_all) > 0)>::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP, class S >
void
convert(const char *from, char *to, S size,
        typename oos::enable_if<((CP & convert_all) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  if (strlen(from) < (size_t)size) {
#ifdef WIN32
    strcpy_s(to, size, from);
#else
    strcpy(to, from);
#endif
  } else {
    throw std::bad_cast();
  }
}

/*
 * from
 *   floating point
 * to
 *   string
 */
template < int CP, class T, class U, class P >
void
convert(const T &from, U &to, P precision,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_same<U, std::string>::value ||
                                CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<P>::value>::type* = 0)
{
  char buf[256];
  convert<CP>(from, buf, 256, precision);
  to.assign(buf);
}

template < int CP, class T, class U, class P >
void
convert(const T &, U &, P ,
        typename oos::enable_if<((CP & convert_strict) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_same<U, std::string>::value ||
                                CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<P>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   integral
 * to
 *   string/varchar
 */
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_same<U, std::string>::value ||
                                CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value>::type* = 0)
{
  char buf[256];
  convert<CP>(from, buf, 256);
  to.assign(buf);
}
template < int CP, class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_same<U, std::string>::value ||
                                CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, U>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0)
{
  throw std::bad_cast();
}

/*
 * from
 *   const char array
 * to
 *   signed integral
 */
template < int CP, class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<(((CP & convert_fitting_weak) > 0))>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_signed<T>::value)>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtol(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   unsigned integral
 */
template < int CP, class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<(((CP & convert_fitting_weak) > 0))>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_integral<T>::value &&
                                 CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value &&
                                 !CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value)>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtoul(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   floating point
 */
template < int CP, class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<(((CP & convert_fitting_weak) > 0))>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtod(from, &ptr));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const char array
 * to
 *   bool
 */
template < int CP >
void
convert(const char *from, bool &to,
        typename oos::enable_if<(((CP & convert_fitting_weak) > 0))>::type* = 0)
{
  char *ptr;
  to = strtoul(from, &ptr, 10) > 0;
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}
/*
 * from
 *   const string
 * to
 *   signed integral
 */
template < int CP, class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<(((CP & convert_fitting_weak) > 0))>::type* = 0,
        typename oos::enable_if<(CPP11_TYPE_TRAITS_NS::is_arithmetic<U>::value &&
                                 (CPP11_TYPE_TRAITS_NS::is_same<T, std::string>::value ||
                                  CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, T>::value))>::type* = 0)
{
  convert<CP>(from.c_str(), to);
}

template < int CP, class T >
void
convert(const T&, char*,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP, class T, class U, class S >
void
convert(const T &, U &, S ,
        typename oos::enable_if<((CP & convert_all) > 0)>::type* = 0,
        typename oos::enable_if<!((CPP11_TYPE_TRAITS_NS::is_same<U, std::string>::value ||
                                   CPP11_TYPE_TRAITS_NS::is_base_of<varchar_base, U>::value) &&
                                  CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP, class T, class U, class S, class P >
void
convert(const T &, U &, S , P ,
        typename oos::enable_if<((CP & convert_all) > 0)>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<P>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP >
void
convert(const varchar_base &from, std::string &to,
        typename oos::enable_if<CP == convert_strict>::type* = 0)
{
  throw std::bad_cast();
}

template < int CP >
void
convert(const varchar_base &from, std::string &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  to = from.str();
}

template < int CP >
void
convert(const std::string &from, varchar_base &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  to = from;
}

template < int CP >
void
convert(const std::string &from, std::string &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  to = from;
}

template < int CP >
void
convert(const varchar_base &from, varchar_base &to,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  to = from;
}

template < class T, class U >
void
convert(const T &from, U &to)
{
  convert<convert_fitting>(from, to);
}

template < class T, class U, class S >
void
convert(const T &from, U &to, S size,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0)
{
  convert<convert_fitting>(from, to, size);
}

template < class T, class U, class S, class P >
void
convert(const T &from, U &to, S size, P precision,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<S>::value>::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<P>::value>::type* = 0)
{
  convert<convert_fitting>(from, to, size, precision);
}

#endif /* OOS_DOXYGEN_DOC */

}

#endif /* CONVERT_HPP */
