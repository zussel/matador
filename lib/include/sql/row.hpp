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

#ifndef ROW_HPP
#define ROW_HPP

#ifdef _MSC_VER
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

#include "sql/value.hpp"

#include <cstddef>
#include <vector>

namespace oos {

/// @cond OOS_DEV
class OOS_API row_impl
{
public:
  virtual ~row_impl();
};

class OOS_API row
{
public:
  row();
  ~row();

  /**
   * Add a new column value.
   *
   * @tparam T Type of the column value.
   * @param val The column value.
   */
  template < class T >
  void push_back(const T &val)
  {
    values_.push_back(value<T>(val));
  }

//  void push_back(const std::string &val)
//  {
//    values_.push_back(value<std::string>(val));
//  }

  /**
   * Get value of column position
   *
   * @tparam T Type of column
   * @param pos Column index
   * @return The value of the requested column.
   */
  template < class T >
  T at(size_t pos)
  {
    return values_.at(pos).get<T>();
  }

  std::string str(size_t pos)
  {
    return values_.at(pos).value();
  }

private:

  std::vector<detail::basic_value> values_;
};
/// @endcond

}

#endif /* ROW_HPP */
