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
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_SQL_API
#endif

#include "matador/sql/value.hpp"

#include <cstddef>
#include <vector>
#include <unordered_map>

namespace matador {

/**
 * @brief Row representation
 */
class OOS_SQL_API row
{
public:
  row() = default;
  /**
   * Copy constructs a row from a given row
   *
   * @param r The roe to copy from
   */
  row(const row &r) = default;

  /**
   * Copy moves a row from a given row
   *
   * @param r The row to move from
   */
  row(row &&r) = default;

  /**
   * Copy assigns a row with a given row
   *
   * @param r Row to copy assign from
   * @return Reference to assigned row
   */
  row& operator=(const row &r) = default;

  /**
   * Copy moves a row with a given row
   *
   * @param r Row to copy moves from
   * @return Reference to moved row
   */
  row& operator=(row &&r) = default;
  ~row() = default;

  /**
   * Add a column to the row. If column
   * already exists
   *
   * @param column Name of the column
   * @return True if column was added, false if column already exists
   */
  bool add_column(const std::string &column);

  /**
   * Add a column to the row. If column
   * already exists
   *
   * @param column Name of the column
   * @param value Value of the column
   * @return True if column was added, false if column already exists
   */
  bool add_column(const std::string &column, const std::shared_ptr<value> &value);

  /**
   * @brief Checks if the row has a column of the given name
   * @param column The name of the column to be checked
   * @return True if the column exists
   */
  bool has_column(const std::string &column) const;

  /**
   * @brief Serializes the row with the given serializer
   *
   * @tparam SERIALIZER The type of the used serializer object
   * @param serializer The serializer to be used
   */
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
   * @param val Value to set
   */
  template < class T >
  void set(size_t index, const T &val)
  {
    values_.at(columns_.at(index)).reset(new value(val));
  }

  /**
   * Set a value for a column identified
   * by its name
   *
   * @throw out_of_bound exception
   *
   * @tparam Type of value
   * @param column Name of column to set value for
   * @param val Value to set
   */
  template < class T >
  void set(const std::string &column, const T &val)
  {
    values_.at(column).reset(new value(val));
  }

  /**
   * @brief Sets the given value to the column with given name
   *
   * @param column The name of the column to be set
   * @param value  The new value of the column
   */
  void set(const std::string &column, const std::shared_ptr<value> &value);

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
    return values_.at(columns_.at(pos))->str();
  }

  /**
   * Get string value of column at position
   *
   * @param column Column name
   * @return The string value of the requested column.
   */
  std::string str(const std::string &column)
  {
    return values_.at(column)->str();
  }

  /**
   * @brief Clear the row
   *
   * Clear the row and all its columns
   */
  void clear();

private:
  typedef std::shared_ptr<value> value_ptr;
  typedef std::vector<std::string> t_columns;
  typedef std::unordered_map<std::string, value_ptr> t_values;

  t_columns columns_;
  t_values values_;

};
/// @endcond

}

#endif /* ROW_HPP */
