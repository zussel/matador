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

#include <type_traits>

#include <iostream>

namespace oos {

class varchar_base;
class object_base_ptr;

template < typename T, typename U >
void
convert(const T &from, U &to);

template < typename T >
typename std::enable_if<std::is_integral<T>::value >::type
convert(const char *from, T &to)
{
  std::cout << "int\n";
}

template < typename T >
typename std::enable_if<std::is_floating_point<T>::value >::type
convert(const char *from, T &to)
{
  std::cout << "float\n";
}

template < typename T >
void
convert(const std::string &from, T &to)
{
  convert(from.c_str(), to);
}

template < unsigned int S, typename T >
void
convert(const varchar<S> &from, T &to)
{
  convert(from.str().c_str(), to);
}

}

#endif /* CONVERT_HPP */
