//
// Created by sascha on 3/2/16.
//

#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/token.hpp"
#include "sql/value.hpp"
#include "sql/basic_dialect.hpp"

namespace oos {

/**
 * @brief Represents a database column.
 *
 * Represents a database column consisting of
 * name.
 */
struct OOS_API column : public detail::token
{
  /**
   * @brief Creates a new column with given name
   *
   * @param col The name of the column
   */
  column(const std::string &col);

  /**
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor obejct to be accepted
   */
  virtual void accept(token_visitor &visitor) override;

  std::string name; /**< Name of the column */
};

/**
 * @brief Represents a list of database columns.
 */
struct OOS_API columns : public detail::token
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
   * @param cols The list of columns
   * @param with_brackets The bracket type
   */
  columns(std::initializer_list<column> cols, t_brackets with_brackets = WITH_BRACKETS);

  /**
   * @brief Creates an empty list of columns.
   *
   * @param with_brackets The bracket type
   */
  explicit columns(t_brackets with_brackets = WITH_BRACKETS);

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
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor obejct to be accepted
   */
  virtual void accept(token_visitor &visitor);

  std::vector<std::shared_ptr<column>> columns_; /**< The list of column shared pointer */
  t_brackets with_brackets_ = WITH_BRACKETS;     /**< The bracket type */

  static columns all_;                           /**< The all columns object */
};

namespace detail {

/// @cond OOS_DEV

struct OOS_API typed_column : public oos::column
{
  typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host);

  virtual void accept(token_visitor &visitor) override;

  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct OOS_API identifier_column : public typed_column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : typed_column(n, t, idx, host) { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct OOS_API typed_varchar_column : public typed_column
{
  typed_varchar_column(const char *n, size_t size, data_type_t t, size_t idx, bool host)
    : typed_column(n, t, idx, host)
    , size(size)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  size_t size;
};

struct OOS_API identifier_varchar_column : public typed_varchar_column
{
  identifier_varchar_column(const char *n, size_t size, data_type_t t, size_t idx, bool host)
    : typed_varchar_column(n, size, t, idx, host)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct OOS_API basic_value_column : public column
{
  basic_value_column(const std::string &col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  basic_value_column(const char *col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }

  std::unique_ptr<basic_value> value_;
};

template < class T >
struct value_column : public basic_value_column
{

  value_column(const std::string &col, T& val)
    : basic_value_column(col, new value<T>(val))
  { }

  value_column(const char *col, T& val)
  : basic_value_column(col, new value<T>(val))
  { }
};

template <>
struct value_column<char*> : public basic_value_column
{
  value_column(const std::string &col, char*& val, size_t s)
    : basic_value_column(col, new value<char*>(val, s))
  { }

  value_column(const char *col, char*& val, size_t s)
  : basic_value_column(col, new value<char*>(val, s))
  { }
};

/// @endcond

}

}
#endif //OOS_COLUMN_HPP
