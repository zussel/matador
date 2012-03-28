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

#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

namespace oos {

template< class InputIterator, class Function, class Predicate >
Function for_each_if(InputIterator first, InputIterator last, Predicate pred, Function f)
{
  for( ; first != last; ++first) {
    if( pred(*first) ) {
      f(*first);
    }
  }
  return f;
}

template< class InputIterator, class Function, class T>
Function for_each_equal(InputIterator first, InputIterator last, const T& value, Function f)
{
  for( ; first != last; ++first) {
    if( *first == value ) {
      f(*first);
    }
  }
  return f;
}

}

#endif /* ALGORITHM_HPP */
