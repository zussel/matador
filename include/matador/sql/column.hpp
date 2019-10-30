//
// Created by sascha on 3/2/16.
//

#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/token.hpp"
#include "matador/sql/value.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

/**
 * @brief Represents a database column.
 *
 * Represents a database column consisting of
 * name.
 */
struct OOS_SQL_API column : public detail::token
{
  /**
   * @brief Creates a new column with given name
   *
   * @param col The name of the column
   */
  explicit column(const std::string &col);

  /**
   * @brief Creates a new column with given name
   *
   * @param col The name of the column
   * @param skipquotes True if the column shouldn't get quotes
   */
  column(const std::string &col, bool skipquotes);

  /**
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor obejct to be accepted
   */
  void accept(token_visitor &visitor) override;

  std::string name;         /**< Name of the column */
  bool skip_quotes = false; /**< If true skip quoting */
};

/**
 * @brief Represents a list of database columns.
 */
struct OOS_SQL_API columns : public detail::token
{
  /**
   * Enum declaring values on howto interprete
   * the column list
   */
  enum t_brackets {
    WITH_BRACKETS,   /**< Interpret the columns with surrounding bracktes */
    WITHOUT_BRACKETS /**< Interpret the columns without surrounding bracktes */
  };

  /**
   * @brief Create a list of columns containing given columns and bracket type
   *
   * @param colnames The list of column names
   * @param with_brackets The bracket type
   */
  columns(const std::initializer_list<std::string> &colnames, t_brackets with_brackets = WITH_BRACKETS);

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
   * @param visitor The visitor obejct to be accepted
   */
  void accept(token_visitor &visitor) override;

  std::vector<std::shared_ptr<column>> columns_; /**< The list of column shared pointer */
  t_brackets with_brackets_ = WITH_BRACKETS;     /**< The bracket type */

private:
  static columns all_;                           /**< The all columns object */
  static column count_all_;                      /**< An count all column object */
};

OOS_SQL_API column operator "" _col(const char *name, size_t len);

namespace detail {
struct typed_column;
struct typed_identifier_column;
struct typed_varchar_column;
}

/**
 * @brief Create a typed column object
 *
 * Create a typed column object with the given
 * name and data type of the column
 *
 * @tparam T The data type of the column
 * @param col The name of the column
 * @return The typed_column object.
 */
template < class T >
std::shared_ptr<detail::typed_column> make_typed_column(const std::string &col)
{
  return std::make_shared<detail::typed_column>(col, data_type_traits<T>::type());
}

/**
 * @brief Create a varchar typed column object
 *
 * Create a varchar typed column object with the given
 * name and varchar size of the column
 *
 * @param col The name of the column
 * @param size Size of the varchar
 * @return The varchar typed column
 */
OOS_SQL_API std::shared_ptr<detail::typed_column> make_typed_varchar_column(const std::string &col, size_t size);

/**
 * @brief Create a identifier typed column object
 *
 * Create a identifier typed column object with the given
 * name of the column
 *
 * @tparam T The identifier type
 * @param col The name of the column
 * @return The identifier typed column
 */
template < class T >
std::shared_ptr<detail::typed_column> make_typed_id_column(const std::string &col)
{
  return std::make_shared<detail::typed_identifier_column>(col, data_type_traits<T>::type());
}

namespace detail {

/// @cond MATADOR_DEV

struct OOS_SQL_API typed_column : public matador::column
{
  typed_column(const std::string &col, data_type t);
  typed_column(const std::string &col, data_type t, std::size_t idx, bool host);

  void accept(token_visitor &visitor) override;

  data_type type;
  std::size_t index = 0;
  bool is_host = false;
};

struct OOS_SQL_API typed_identifier_column : public typed_column
{
  typed_identifier_column(const std::string &n, data_type t) : typed_column(n, t) { }
  typed_identifier_column(const std::string &n, data_type t, size_t idx, bool host) : typed_column(n, t, idx, host) { }

  void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct OOS_SQL_API typed_varchar_column : public typed_column
{
  typed_varchar_column(const std::string &n, size_t size, data_type t)
    : typed_column(n, t)
    , size(size)
  { }
  typed_varchar_column(const std::string &n, size_t size, data_type t, size_t idx, bool host)
    : typed_column(n, t, idx, host)
    , size(size)
  { }

  void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  size_t size;
};

struct OOS_SQL_API identifier_varchar_column : public typed_varchar_column
{
  identifier_varchar_column(const char *n, size_t size, data_type t, size_t idx, bool host)
    : typed_varchar_column(n, size, t, idx, host)
  { }

  void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct OOS_SQL_API basic_value_column : public column
{
  basic_value_column(const std::string &col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  basic_value_column(const char *col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }

  std::unique_ptr<basic_value> value_;
};

template < class T >
struct value_column : public basic_value_column
{
  value_column(const std::string &col, basic_value *val)
    : basic_value_column(col, val)
  { }

  value_column(const std::string &col, const T& val)
    : basic_value_column(col, new value<T>(val))
  { }

  value_column(const char *col, const T& val)
  : basic_value_column(col, new value<T>(val))
  { }
};

template <>
struct value_column<char*> : public basic_value_column
{
  value_column(const std::string &col, basic_value *val)
  : basic_value_column(col, val)
  { }

  value_column(const std::string &col, char* val, size_t s)
    : basic_value_column(col, new value<char*>(val, s))
  { }

  value_column(const char *col, char* val, size_t s)
  : basic_value_column(col, new value<char*>(val, s))
  { }
};

template <>
struct value_column<const char*> : public basic_value_column
{
  value_column(const std::string &col, basic_value *val)
  : basic_value_column(col, val)
  { }

  value_column(const std::string &col, const char* val, size_t s)
    : basic_value_column(col, new value<char*>(val, s))
  { }

  value_column(const char *col, const char* val, size_t s)
  : basic_value_column(col, new value<char*>(val, s))
  { }
};

//template <>
//struct value_column<std::string> : public basic_value_column
//{
//  value_column(const std::string &col, basic_value *val)
//  : basic_value_column(col, val)
//  { }
//
//  value_column(const std::string &col, const std::string &val, size_t)
//    : basic_value_column(col, new value<std::string>(val))
//  { }
//
//  value_column(const char *col, const std::string &val, size_t)
//  : basic_value_column(col, new value<std::string>(val))
//  { }
//};

/// @endcond

}

}
#endif //OOS_COLUMN_HPP
