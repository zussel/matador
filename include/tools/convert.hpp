/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONVERT_HPP
#define CONVERT_HPP

#include "tools/varchar.hpp"
#include "tools/enable_if.hpp"

#include <type_traits>
#include <cstdlib>
#include <iostream>
#include <typeinfo>

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

namespace oos {

/*******************************************
 * 
 * Convert from
 *    signed integral T
 * to
 *    signed integral U
 * where
 *    size of T is smaller than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "SUCCEEDED: not same, signed (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  to = from;
}

/*******************************************
 * 
 * Convert from
 *    unsigned integral T
 * to
 *    unsigned integral U
 * where
 *    size of T is smaller than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "SUCCEEDED: not same, unsigned (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  to = from;
}

/*******************************************
 * 
 * Convert from
 *    signed integral T
 * to
 *    unsigned integral U
 * where
 *    size of T is smaller than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "FAILED: not convertible, not same, signed > unsigned (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Convert from
 *    unsigned integral T
 * to
 *    signed integral U
 * where
 *    size of T is smaller than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<(sizeof(T) < sizeof(U))>::type* = 0)
{
  // cout << "SUCCEEDED: not same, unsigned > signed (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  to = from;
}

/*******************************************
 * 
 * Cannot convert from
 *    signed integral T
 * to
 *    signed integral U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  //cout << "FAILED: not convertible, not same, signed (" << sizeof(T) << " > " << sizeof(U) << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Cannot convert from
 *    unsigned integral T
 * to
 *    unsigned integral U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value >::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "FAILED: not convertible, not same, unsigned (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Cannot convert from
 *    signed integral T
 * to
 *    unsigned integral U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<U, bool>::value >::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "FAILED: not convertible, not same, signed > unsigned (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Cannot convert from
 *    unsigned integral T
 * to
 *    signed integral U
 * where
 *    size of T is greater than size of U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) < sizeof(U))>::type* = 0)
{
  // cout << "FAILED: not convertible, not same, unsigned > signed (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
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
template < class T, class U >
void
convert(const T &from, U &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value >::type* = 0,
        typename oos::enable_if<!(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "SUCCEEDED: not same, floating point (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  to = static_cast<T>(from);
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
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value >::type* = 0,
        typename oos::enable_if<(sizeof(T) > sizeof(U))>::type* = 0)
{
  // cout << "FAILED: not convertible, not same, floating point (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Cannot convert from
 *    integral T
 * to
 *    floating point U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<U>::value >::type* = 0)
{
  // cout << "FAILED: not convertible, integral > floating point (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/*******************************************
 * 
 * Cannot convert from
 *    floating point T
 * to
 *    integral U
 *
 *******************************************/
template < class T, class U >
void
convert(const T &, U &,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<U>::value >::type* = 0)
{
  // cout << "FAILED: not convertible, floating point > integral (" << typeid(T).name() << " > " << typeid(U).name() << ")\n";
  throw std::bad_cast();
}

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   signed integral
 *
 ***********************************************/
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtol(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    // cout << "FAILED: const char* > signed integral (" << typeid(T).name() << ")\n";
    throw std::bad_cast();
  } else {
    // cout << "SUCCEEDED: const char* > signed integral (" << typeid(T).name() << ")\n";
  }
}

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   bool
 *
 ***********************************************/
void
convert(const char *from, bool &to);

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   unsigned integral
 *
 ***********************************************/
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtoul(from, &ptr, 10));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    // cout << "FAILED: const char* > unsigned integral (" << typeid(T).name() << ")\n";
    throw std::bad_cast();
  } else {
    // cout << "SUCCEEDED: const char* > unsigned integral (" << typeid(T).name() << ")\n";
  }
}

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   floating point
 *
 ***********************************************/
template < class T >
void
convert(const char *from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  char *ptr;
  to = static_cast<T>(strtod(from, &ptr));
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    // cout << "FAILED: const char* > floating point (" << typeid(T).name() << ")\n";
    throw std::bad_cast();
  } else {
    // cout << "SUCCEEDED: const char* > floating point (" << typeid(T).name() << ")\n";
  }
}

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   string
 *
 ***********************************************/
void
convert(const char *from, std::string &to);

/***********************************************
 * 
 * Convert from
 *   varchar
 * to
 *   string
 *
 ***********************************************/
void
convert(const oos::varchar_base &from, std::string &to);

/***********************************************
 * 
 * Convert from 
 *   string
 * to
 *   char*
 *
 ***********************************************/
void
convert(const std::string &from, char *to, size_t num);

/***********************************************
 * 
 * Convert from 
 *   varchar
 * to
 *   char*
 *
 ***********************************************/
void
convert(const oos::varchar_base &from, char *to, size_t num);

/***********************************************
 * 
 * Convert from 
 *   char
 * to
 *   char*
 *
 ***********************************************/
void
convert(const char &from, char *to, size_t num);

/***********************************************
 * 
 * Convert from 
 *   signed integral
 * to
 *   char*
 *
 ***********************************************/
template < class T >
void
convert(const T &from, char *to, size_t num,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_signed<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, char>::value >::type* = 0)
{
  // cout << "SUCCEEDED: signed integral > char* (" << typeid(T).name() << ")\n";
#ifdef WIN32
  _snprintf_s(to, num, num, "%d", from);
#else
  snprintf(to, num, "%ld", (long)from);
#endif
}

/***********************************************
 * 
 * Convert from 
 *   unsigned integral
 * to
 *   char*
 *
 ***********************************************/
template < class T >
void
convert(const T &from, char *to, size_t num,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_unsigned<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, char>::value >::type* = 0)
{
  // cout << "SUCCEEDED: unsigned integral > char* (" << typeid(T).name() << ")\n";
#ifdef WIN32
  _snprintf_s(to, num, num, "%d", from);
#else
  snprintf(to, num, "%lu", (long unsigned int)from);
#endif
}

/***********************************************
 * 
 * Convert from 
 *   floating point
 * to
 *   char*
 *
 ***********************************************/
template < class T >
void
convert(const T &from, char *to, size_t num, int precision,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  // cout << "SUCCEEDED: floating point > char* (" << typeid(T).name() << ")\n";
  char buf[32];
#ifdef WIN32
  _snprintf_s(buf, 32, 32, "%%0.%df", precision);
  _snprintf_s(to, num, num, buf, from);
#else
  snprintf(buf, 32, "%%0.%df", precision);
  snprintf(to, num, buf, from);
#endif
}

/***********************************
 * 
 * Convert from
 *   string
 * to
 *   arithmetic
 *
 ***********************************/
template < class T >
void
convert(const std::string &from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value >::type* = 0)
{
  // cout << "DELEGATED: string > arithmetic (" << typeid(T).name() << ")\n";
  convert(from.c_str(), to);
}

/***********************************
 * 
 * Convert from
 *   integral
 * to
 *   string
 *
 ***********************************/
template < class T >
void
convert(const T &from, std::string &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0)
{
  // cout << "DELEGATED: integral > string (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256);
  to.assign(buf);
}

/***********************************
 * 
 * Convert from
 *   floating point
 * to
 *   string
 *
 ***********************************/
template < class T >
void
convert(const T &from, std::string &to, int precision,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  // cout << "DELEGATED: floating point > string (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256, precision);
  to.assign(buf);
}

/***********************************
 * 
 * Convert from
 *   varchar
 * to
 *   arithmetic
 *
 ***********************************/
template < class T >
void
convert(const oos::varchar_base &from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value >::type* = 0)
{
  // cout << "DELEGATED: string > arithmetic (" << typeid(T).name() << ")\n";
  convert(from.c_str(), to);
}

/***********************************
 * 
 * Convert from
 *    bool
 * to
 *    arithmetic
 *
 ***********************************/

template < class T >
void
convert(const bool &from, T &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0)
{
  // cout << "SUCCEEDED: bool > arithmetic (" << typeid(T).name() << ")\n";
  to = from;
}

/***********************************
 * 
 * Convert from
 *    arithmetic
 * to
 *    bool
 *
 ***********************************/

template < class T >
void
convert(const T &from, bool &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_arithmetic<T>::value >::type* = 0,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, bool>::value >::type* = 0)
{
  // cout << "SUCCEEDED: arithmetic > bool (" << typeid(T).name() << ")\n";
  to = from > 1;
}

/***********************************
 * 
 * Convert same type
 *
 ***********************************/
template < class T >
void
convert(const T &from, T &to)
{
  // cout << "SUCCEEDED: same (" << typeid(T).name() << " > " << typeid(T).name() << ")\n";
  to = from;
}

/***********************************************
 * 
 * Convert from
 *   string
 * to
 *   varchar
 *
 ***********************************************/
void
convert(const std::string &from, oos::varchar_base &to);

/***********************************************
 * 
 * Convert from
 *   const char*
 * to
 *   varchar
 *
 ***********************************************/
void
convert(const char *from, oos::varchar_base &to);

/***********************************
 * 
 * Convert from
 *   integral
 * to
 *   varchar
 *
 ***********************************/
template < class T >
void
convert(const T &from, oos::varchar_base &to,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value >::type* = 0)
{
  // cout << "DELEGATED: integral > varchar (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256);
  to.assign(buf);
}

/***********************************
 * 
 * Convert from
 *   floating point
 * to
 *   varchar
 *
 ***********************************/
template < class T >
void
convert(const T &from, oos::varchar_base &to, int precision,
        typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value >::type* = 0)
{
  // cout << "DELEGATED: floating point > varchar (" << typeid(T).name() << ")\n";
  char buf[256];
  convert(from, buf, 256, precision);
  to.assign(buf);
}

/***********************************
 * 
 * Convert from
 *   varchar
 * to
 *   varchar
 *
 ***********************************/
void
convert(const oos::varchar_base &from, oos::varchar_base &to);

}

#endif /* CONVERT_HPP */
