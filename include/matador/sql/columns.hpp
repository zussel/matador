#ifndef MATADOR_COLUMNS_HPP
#define MATADOR_COLUMNS_HPP

#include "matador/sql/column.hpp"
#include "matador/sql/token.hpp"

namespace matador {

/**
 * @brief Represents a list of database columns.
 */
struct columns : public detail::token
{
  /**
   * Enum declaring values on howto interpret
   * the column list
   */
  enum t_brackets {
    WITH_BRACKETS,   /**< Interpret the columns with surrounding brackets */
    WITHOUT_BRACKETS /**< Interpret the columns without surrounding brackets */
  };

  /**
   * @brief Create a list of columns containing given columns and bracket type
   *
   * @param column_names The list of column names
   * @param with_brackets The bracket type
   */
  columns(const std::initializer_list<std::string> &column_names, t_brackets with_brackets = WITH_BRACKETS);

  /**
   * @brief Create a list of columns containing given columns and bracket type
   *
   * @param column_names The list of column names
   * @param with_brackets The bracket type
   */
  explicit columns(const std::vector<std::string> &column_names, t_brackets with_brackets = WITH_BRACKETS);

  /**
   * @brief Create a list of columns containing given columns and bracket type
   *
   * @param cols The list of columns
   * @param with_brackets The bracket type
   */
  columns(std::initializer_list<column> cols, t_brackets with_brackets = WITH_BRACKETS);

  /**
   * @brief Create a list of columns containing given columns and bracket type
   *
   * @param cols The list of columns
   * @param with_brackets The bracket type
   */
  columns(std::initializer_list<std::shared_ptr<column>> cols, t_brackets with_brackets = WITH_BRACKETS);

  /**
   * @brief Creates an empty list of columns.
   *
   * @param with_brackets The bracket type
   */
  explicit columns(t_brackets with_brackets = WITH_BRACKETS);

  /**
   * Copy column
   *
   * @param x Column to copy
   */
  columns(const columns &x);

  /**
   * Copy assign column
   *
   * @param x Column to copy
   * @return Reference to copied column
   */
  columns& operator=(const columns &x);

  /**
   * @brief Append a column to the list
   * @param col The column to be appended
   */
  void push_back(const std::shared_ptr<column> &col);

  /**
   * @brief Sets columns to be interpreted with surrounding brackets.
   *
   * Sets columns to be interpreted with surrounding
   * brackets. After set a references to this is returned.
   *
   * @return A reference to this
   */
  columns & with_brackets();

  /**
   * @brief Sets columns to be interpreted without surrounding brackets.
   *
   * Sets columns to be interpreted without surrounding
   * brackets. After set a references to this is returned.
   *
   * @return A reference to this
   */
  columns & without_brackets();

  /**
   * @brief Creates a columns object representing all columns.
   *
   * @return An all columns representing columns object
   */
  static columns all();

  /**
   * @brief A shortcut for a count all column
   * @return Count all column
   */
  static column count_all();

  /**
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor object to be accepted
   */
  void accept(token_visitor &visitor) override;

  std::vector<std::shared_ptr<column>> columns_; /**< The list of column shared pointer */
  t_brackets with_brackets_ = WITH_BRACKETS;     /**< The bracket type */

private:
  static columns all_;                           /**< The all columns object */
  static column count_all_;                      /**< An count all column object */
};

}

#endif //MATADOR_COLUMNS_HPP
