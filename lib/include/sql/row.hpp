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

/**
 * @brief Row representation
 */
class OOS_API row
{
public:
  row();
  ~row();

  /**
   * Add a column to the row. If column
   * already exists
   *
   * @param column Name of the column
   * @return True if column was added, false if column aready exists
   */
  bool add_column(const std::string &column);

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    for (auto &&column : columns_) {
      values_.at(column)->serialize(column.c_str(), serializer);
    }
  }
  /**
   * Set a value for a column at
   * given index
   *
   * @throw out_of_bound exception
   *
   * @tparam Type of value
   * @param index Index of column to set value for
   * @param value to set
   */
  template < class T >
  void set(size_t index, const T &val)
  {
    values_.at(columns_.at(index)).reset(new value<T>(val));
  }

  /**
   * Set a value for a column identified
   * by its name
   *
   * @throw out_of_bound exception
   *
   * @tparam Type of value
   * @param column Name of column to set value for
   * @param value to set
   */
  template < class T >
  void set(const std::string &column, const T &val)
  {
    values_.at(column).reset(new value<T>(val));
  }

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
    return values_.at(columns_.at(pos))->get<T>();
  }

  /**
   * Get value of column identified by name
   *
   * @tparam T Type of column
   * @param column Column name
   * @return The value of the requested column.
   */
  template < class T >
  T at(const std::string &column)
  {
    return values_.at(column)->get<T>();
  }

  /**
   * Get string value of column at position
   *
   * @param pos Column index
   * @return The string value of the requested column.
   */
  std::string str(size_t pos)
  {
    return values_.at(columns_.at(pos))->value();
  }

private:

//  std::vector<detail::basic_value> values_;

  typedef std::shared_ptr<detail::basic_value> value_ptr;
  typedef std::vector<std::string> t_columns;
  typedef std::unordered_map<std::string, value_ptr> t_values;

  t_columns columns_;
  t_values values_;

};
/// @endcond

}

#endif /* ROW_HPP */
