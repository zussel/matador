#ifndef ROW_HPP
#define ROW_HPP

#include "matador/sql/value.hpp"
#include "matador/sql/value_processor.hpp"

#include <cstddef>
#include <memory>
#include <vector>
#include <unordered_map>

namespace matador {

/**
 * @brief Row representation
 */
class row
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
  row(row &&r) noexcept = default;

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
   * @tparam Serializer The type of the used serializer object
   * @param serializer The serializer to be used
   */
  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    for (auto &column : columns_) {
      value_processor_.apply(values_.at(column)->value_, column.c_str(), serializer);
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
    values_.at(columns_.at(index)) = std::make_shared<value>(val);
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
    values_.at(column) = std::make_shared<value>(val);
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
    return value_to_string_visitor_.to_string(*values_.at(columns_.at(pos)));
  }

  /**
   * Get string value of column at position
   *
   * @param column Column name
   * @return The string value of the requested column.
   */
  std::string str(const std::string &column)
  {
    return value_to_string_visitor_.to_string(*values_.at(column));
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

  detail::value_processor value_processor_;
  detail::value_to_string_processor value_to_string_visitor_;
};
/// @endcond

}

#endif /* ROW_HPP */
