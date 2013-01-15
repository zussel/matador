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

#ifndef CONDITIONAL_HPP
#define CONDITIONAL_HPP

namespace oos {

/// @cond OOS_DEV

template<bool B, class T, class F>
struct conditional { typedef T type; };
 
template<class T, class F>
struct conditional<false, T, F> { typedef F type; };

/// @endcond OOS_DEV

}

#endif /* CONDITIONAL_HPP */
