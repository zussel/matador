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

namespace oos {

class varchar_base;
class object_base_ptr;

class convert2
{
private:
  convert2();
  convert2(const convert2&);
  convert2& operator=(const convert2&);
  ~convert2();

public:
  template < class T >
  static T to_value(const std::string &str)
  {
    return convert2::to_value(str.c_str());
  }

  template < class T >
  static T to_value(const varchar_base &str)
  {
    return convert2::to_value(str.c_str());
  }

  template < class T >
  static T to_value(const char *str);

  static bool to_value(const char *str)
  {
    return std::strtol(str, 0, 10) > 0;
  }

  template < class T >
  static T to_value(const char *str,
                  typename oos::enable_if<!std::tr1::is_integral<T>::value >::type* = 0,
                  typename oos::enable_if<!std::tr1::is_signed<T>::value >::type* = 0)
  {
    return static_cast<T>(std::strtol(str, 0, 10));
  }

  template < class T >
  static T to_value(const char *str,
                  typename oos::enable_if<!std::tr1::is_integral<T>::value >::type* = 0,
                  typename oos::enable_if<!std::tr1::is_unsigned<T>::value >::type* = 0)
  {
    return static_cast<T>(std::strtoul(str, 0, 10));
  }

  template < class T >
  static T to_value(const char *str,
                   typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type* = 0)
  {
    return static_cast<T>(std::strtod(str, 0));
  }

  template < class T >
  std::string to_string(const T &value);

  std::string to_string(const bool &value)
  {
    return (value ? "true" : "false");
  }

  template < class T >
  std::string to_string(const T &value,
                        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
                        typename oos::enable_if<!std::tr1::is_signed<T>::value >::type* = 0)
  {
    char buf[256];
    sprintf(buf, "%d", value);
    return buf;
  }

  template < class T >
  std::string to_string(const T &value,
                        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
                        typename oos::enable_if<!std::tr1::is_unsigned<T>::value >::type* = 0)
  {
    char buf[256];
    sprintf(buf, "%u", value);
    return buf;
  }

  template < class T >
  std::string to_string(const T &value,
                        typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type* = 0)
  {
    char buf[256];
    sprintf(buf, "%f", value);
    return buf;
  }
};

template < typename T, typename U >
typename oos::enable_if<std::tr1::is_same<T, U>::value >::type
convert(const T &from, U &to)
{
  to = from;
  std::cout << "same (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_integral<T>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) <= sizeof(U)>::type* = 0)
{
  to = from;
  std::cout << "same (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < typename T, typename U >
void
convert(const T &from, U &to,
        typename oos::enable_if<!std::tr1::is_same<T, U>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type* = 0,
        typename oos::enable_if<sizeof(T) <= sizeof(U)>::type* = 0)
{
  to = from;
  std::cout << "same (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < class T >
typename oos::enable_if<std::tr1::is_same<T, bool>::value >::type
convert(const char *from, T &to)
{
  to = std::strtol(from, 0, 10) > 0;
  std::cout << "bool (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < class T >
typename oos::enable_if<std::tr1::is_integral<T>::value >::type
convert(const char *from, T &to, typename oos::enable_if<std::tr1::is_signed<T>::value >::type* = 0)
{
  to = static_cast<T>(std::strtol(from, 0, 10));
  std::cout << "long (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < class T >
typename oos::enable_if<std::tr1::is_integral<T>::value >::type
convert(const char *from, T &to, typename oos::enable_if<std::tr1::is_unsigned<T>::value >::type* = 0)
{
  to = static_cast<T>(std::strtoul(from, 0, 10));
  std::cout << "unsigned long (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < typename T >
typename oos::enable_if<std::tr1::is_floating_point<T>::value >::type
convert(const char *from, T &to)
{
  to = static_cast<T>(std::strtod(from, 0));
  std::cout << "double (sizeof(to): " << sizeof(T) << ") value: " << to << "\n";
}

template < typename T >
typename oos::enable_if<!std::tr1::is_same<T, std::string>::value >::type
convert(const std::string &from, T &to)
{
  convert(from.c_str(), to);
}

template < unsigned int S1, unsigned int S2 >
void
convert(const varchar<S1> &from, varchar<S2> &to)
{
  to = from;
}

template < unsigned int S, typename T >
void
convert(const varchar<S> &from, T &to)
{
  convert(from.str().c_str(), to);
}

}

#endif /* CONVERT_HPP */
