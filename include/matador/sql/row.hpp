#ifndef ROW_HPP
#define ROW_HPP

#include "matador/sql/column.hpp"

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
  using column_ptr = std::shared_ptr<column>;

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
   * @param col Column to add
   * @return True if column was added, false if column already exists
   */
  bool add_column(const column_ptr &col);

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
  template < class Operator >
  void process(Operator &op)
  {
    for (auto &column : columns_) {
      op.process(column->name().c_str(), column->value(), column->attributes());
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
    columns_.at(index)->value().value_ = val;
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
    columns_by_name_.at(column)->value() = val;
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
    return columns_.at(pos)->value().get<T>();
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
    return columns_by_name_.at(column)->value().get<T>();
  }

  /**
   * @brief Clear the row
   *
   * Clear the row and all its columns
   */
  void clear();

private:
  using column_by_index = std::vector<column_ptr>;
  using column_by_name_map = std::unordered_map<std::string, column_ptr >;

  column_by_index columns_;
  column_by_name_map columns_by_name_;
};
/// @endcond

}

#endif /* ROW_HPP */
