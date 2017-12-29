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

/**
 * @file algorithm.hpp
 * @brief Contains several algorithms and matador functions.
 * 
 * This file contains several functional extensions for the
 * OOS (i.e. for_each_xxx). The are also usable with the STL.
 */

namespace matador {

/**
 * @brief Applies a function if element is valid with given predicate
 * 
 * Applies given function for each element in the sequence which
 * returns true when called with the predicate,
 *
 * @tparam InputIterator Type of iterator to iterate with.
 * @tparam Function Type of the function to call.
 * @tparam Predicate Function or class which must be called with the element
 * @param first The initial iterator containing the starting position for the range.
 * @param last The final iterator containing the ending position for the range.
 * @param pred The predicate called with each element
 * @param f The function called with each element which returned true with the predicate.
 * @return The called function.
 */
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

/**
 * @brief Applies a function if element is equal to the given value
 * 
 * Applies given function for each element in the sequence if
 * the element is equal to the given value,
 *
 * @tparam InputIterator Type of iterator to iterate with.
 * @tparam Function Type of the function to call.
 * @param first The initial iterator containing the starting position for the range.
 * @param last The final iterator containing the ending position for the range.
 * @param value The value to compare with.
 * @param f The function called with each element which is equal to the value.
 * @return The called function.
 */
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
