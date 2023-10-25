#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "matador/sql/types.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/basic_query.hpp"

#include <string>
#include <sstream>
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>
#include <array>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

class basic_condition : public token
{
public:
  basic_condition() : token(token::CONDITION) { }

  enum t_operand
  {
    EQUAL = 0,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    OR,
    AND,
    NOT,
    IN_LIST,
    LIKE
  };

  enum
  {
    num_operands = 11
  };

  void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }

  virtual std::string evaluate(basic_dialect &dialect) const = 0;

  static std::array<std::string, num_operands> operands;
};

class basic_column_condition : public basic_condition
{
public:
  column field_;
  std::string operand;

  basic_column_condition(column fld, detail::basic_condition::t_operand op)
    : field_(std::move(fld)), operand(detail::basic_condition::operands[op])
  { }

  void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }
};

//class OOS_SQL_API basic_in_condition : public basic_condition
class basic_in_condition : public basic_condition
{
public:
  column field_;

  explicit basic_in_condition(column fld)
    : field_(std::move(fld))
  { }

  void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }

  virtual size_t size() const = 0;
};

/// @endcond

}

/**
 * @class condition
 * @brief Represents a sql query condition
 *
 * @tparam L Left hand operator type
 * @tparam R Right hand operator type
 * This class represents a condition part
 * of a sql query or update statement.
 * Each compare method returns a reference to
 * the condition itself. That way one can
 * concatenate conditions together.
 */

/// @cond MATADOR_DEV

template<class L, class R, class Enabled = void>
class condition;

template<class T>
class condition<column, T, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public detail::basic_column_condition
{
public:
  condition(const column &fld, detail::basic_condition::t_operand op, T val)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(basic_dialect &dialect) const override
  {
    dialect.add_host_var(field_.name());
    std::stringstream str;
    if (dialect.compile_type() == basic_dialect::DIRECT) {
      str << dialect.prepare_identifier(field_.name()) << " " << operand << " " << value;
    } else {
      str << dialect.prepare_identifier(field_.name()) << " " << operand << " " << dialect.next_placeholder();
    }
    return str.str();
  }
};

template<class T>
class condition<column, T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public detail::basic_column_condition
{
public:
  condition(const column &fld, detail::basic_condition::t_operand op, T val)
    : basic_column_condition(fld, op)
    ,value(val)
  { }

  T value;

  std::string evaluate(basic_dialect &dialect) const override
  {
    dialect.add_host_var(field_.name());
    std::stringstream str;
    if (dialect.compile_type() == basic_dialect::DIRECT) {
      str << dialect.prepare_identifier(field_.name()) << " " << operand << " '" << value << "'";
    } else {
      str << dialect.prepare_identifier(field_.name()) << " " << operand << " " << dialect.next_placeholder();
    }
    return str.str();
  }
};

template<class T>
class condition<T, column, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public detail::basic_column_condition
{
public:
  condition(T val, detail::basic_condition::t_operand op, const column &fld)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << value << " " << operand << " " << dialect.prepare_identifier(field_.name());
    return str.str();
  }
};

template<class T>
class condition<T, column, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public detail::basic_column_condition
{
public:
  condition(T val, detail::basic_condition::t_operand op, const column &fld)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << "'" << value << "' " << operand << " " << dialect.prepare_identifier(field_.name());
    return str.str();
  }
};

/// @endcond

/**
 * @brief Condition class representing an IN condition
 *
 * This class represents an query IN condition and evaluates to
 * this condition based on the current database dialect
 *
 * @code
 * WHERE age IN (29,34,56)
 * @endcode
 */
template < class V >
class condition<column, std::initializer_list<V>> : public detail::basic_in_condition
{
public:
  /**
   * @brief Creates an IN condition
   *
   * Creates an IN condition for the given column and
   * the given list of arguments.
   *
   * @param col Column for the IN condition
   * @param args List of arguments
   */
  condition(const column &col, const std::initializer_list<V> &args)
    : basic_in_condition(col)
    , args_(args)
  {}

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a part of the
   * query string based on the given compile type
   *
   * @param dialect The dialect used to evaluate
   * @return A condition IN part of the query
   */
  std::string evaluate(basic_dialect &dialect) const override
  {
    auto count = size();
    for (size_t i = 0; i < count; ++i) {
      dialect.add_host_var(field_.name());
    }
    std::stringstream str;
    str << dialect.prepare_identifier(field_.name()) << " IN (";
    if (args_.size() > 1) {
      auto first = args_.begin();
      auto last = args_.end() - 1;
      while (first != last) {
        if (dialect.compile_type() == basic_dialect::DIRECT) {
          str << *first++ << ",";
        } else {
          ++first;
          str << dialect.next_placeholder() << ",";
        }
      }
    }
    if (!args_.empty()) {
      if (dialect.compile_type() == basic_dialect::DIRECT) {
        str << args_.back();
      } else {
        str << dialect.next_placeholder();
      }
    }
    str << ")";
    return str.str();
  }

  /**
   * @brief Returns the number of arguments in the list
   * @return The number of arguments in the list
   */
  size_t size() const override
  {
    return args_.size();
  }

private:
  std::vector<V> args_;
};

/**
 * @brief Condition class representing an IN condition
 *
 * This class represents an query IN condition and evaluates to
 * this condition based on the current database dialect
 *
 * @code
 * WHERE age IN (select age_value from <table>)
 * @endcode
 */
template <>
class condition<column, detail::basic_query> : public detail::basic_column_condition
{
public:
  /**
   * @brief Create a query IN condition
   *
   * Create an IN condition where the argument values come from
   * the given query. To evaluate the query a sql dialect must be
   * given.
   *
   * @param col Column for the IN condition
   * @param op Operand of the condition
   * @param q The query to be evaluated to the IN arguments
   */
  condition(column col, detail::basic_condition::t_operand op, detail::basic_query q)
    : basic_column_condition(std::move(col), op)
    , query_(std::move(q))
  {}

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a part of the
   * query string based on the given compile type
   *
   * @param dialect The dialect used to evaluate
   * @return A condition IN part of the query
   */
  std::string evaluate(basic_dialect &dialect) const override
  {
    std::string result(dialect.prepare_identifier(field_.name()) + " " + operand + " (");
    result += dialect.continue_build(query_.stmt(), dialect.compile_type());
    result += (")");
    return result;
  }

private:
  detail::basic_query query_;
};

/**
 * @brief Between condition.
 *
 * The condition represents a between where clause
 * part.
 *
 * @tparam T The type of the boundary values
 */
template < class T >
class condition<column, std::pair<T, T>> : public detail::basic_condition
{
public:
  /**
   * @brief Create a new between condition
   *
   * @param col The column for the range check
   * @param range The boundary values defining the range
   */
  condition(column col, const std::pair<T, T> &range)
    : field_(std::move(col)), range_(range) { }

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a between part
   * based on the given compile type
   *
   * @param dialect The dialect used to evaluate
   * @return A condition BETWEEN part of the query
   */
  std::string evaluate(basic_dialect &dialect) const override
  {
    dialect.add_host_var(field_.name());
    dialect.add_host_var(field_.name());
    std::stringstream str;
    if (dialect.compile_type() == basic_dialect::DIRECT) {
      str << dialect.prepare_identifier(field_.name()) << " BETWEEN " << range_.first << " AND " << range_.second;
    } else {
      str << dialect.prepare_identifier(field_.name()) << " BETWEEN " << dialect.next_placeholder() << " AND " << dialect.next_placeholder();
    }
    return str.str();
  }

private:
  column field_;
  std::pair<T, T> range_;
};

/**
 * @brief Logical binary condition
 *
 * This class represents a logical binary condition
 * - AND
 * - OR
 *
 * @tparam L1 The left hand type of the left operator
 * @tparam R1 The right hand type of the left operator
 * @tparam L2 The left hand type of the right operator
 * @tparam R2 The right hand type of the right operator
 */
template<class L1, class R1, class L2, class R2>
class condition<condition<L1, R1>, condition<L2, R2>> : public detail::basic_condition
{
public:
  /**
   * @brief Create a binary logical condition
   * @param l Left hand operator of the condition
   * @param r right hand operator of the condition
   * @param op The operand (AND or OR)
   */
  condition(const condition<L1, R1> &l, const condition<L2, R2> &r, detail::basic_condition::t_operand op)
    : left(l), right(r), operand(op) { }

  /**
   * @brief Evaluates the condition
   *
   * @param dialect The dialect used to evaluate
   * @return The exaluated string based on the compile type
   */
  std::string evaluate(basic_dialect &dialect) const override
  {
    std::stringstream str;
    // ensure the numbering order for host vars
    auto cl = left.evaluate(dialect);
    auto cr = right.evaluate(dialect);
    if (operand == detail::basic_condition::AND) {
      str << "(" << cl << " " << detail::basic_condition::operands[operand] << " " << cr << ")";
    } else {
      str << cl << " " << detail::basic_condition::operands[operand] << " " << cr;
    }
    return str.str();
  }

  /// @cond MATADOR_DEV
  /**
   * Accept the given visitor for this condition
   * @param visitor Visitor to be accepted
   */
  void accept(token_visitor &visitor) override
  {
    left.accept(visitor);
    right.accept(visitor);
  }
  /// @endcond

private:
  condition<L1, R1> left;
  condition<L2, R2> right;
  detail::basic_condition::t_operand operand;
};

/**
 * @brief Logical unary condition
 *
 * This class represents a logical unary NOT condition
 *
 * @tparam L Left hand type of the condition to be negated
 * @tparam R Right hand type of the condition to be negated
 */
template<class L, class R>
class condition<condition<L, R>, void> : public detail::basic_condition
{
public:
  /**
   * @brief Create a logical unary condition
   * @param c The condition to be negated
   */
  condition(const condition<L, R> &c)
    : cond(c), operand(detail::basic_condition::operands[detail::basic_condition::NOT]) { }

  /**
   * @brief Evaluates the condition
   *
   * @param dialect The dialect used to evaluate
   * @return The exaluated string based on the compile type
   */
  std::string evaluate(basic_dialect &dialect) const override
  {
    std::stringstream str;
    str << operand << " (" << cond.evaluate(dialect) << ")";
    return str.str();
  }

private:
  condition<L, R> cond;
  std::string operand;
};

/**
 * @file condition.hpp
 * @brief Contains functions to create query conditions
 *
 * This file contains some functions to create
 * query conditions. These functions wrap the
 * constructing of a concrete condition and allows
 * expression like condition programming.
 *
 * @code
 * cond1 == cond2
 * cond1 < cond2
 * cond2 != cond1 && cond3 < cond4
 * @endcode
 */

/**
 * @brief Creates an IN condition for a given column and a list of values
 *
 * @tparam V The type of the list arguments
 * @param col The column to compare
 * @param args The list of values
 * @return The condition object
 */
template < class V >
condition<column, std::initializer_list<V>> in(const matador::column &col, std::initializer_list<V> args)
{
  return condition<column, std::initializer_list<V>>(col, args);
}

/**
 * @brief Creates an IN condition for a given column and a query to be executed
 *
 * @param col The column to compare
 * @param q The query to be executes as sub select
 * @return The condition object
 */
condition<column, detail::basic_query> in(const matador::column &col, detail::basic_query &q);
//OOS_SQL_API condition<column, detail::basic_query> in(const matador::column &col, detail::basic_query &q);

/**
 * @brief Creates a between condition.
 *
 * Creates a between condition for the given column with
 * the given range consisting of a low and high value
 *
 * @tparam T The type of the column and range
 * @param col The column for the between condition
 * @param low The low value of the range
 * @param high The high value of the range
 * @return The condition object
 */
template<class T>
condition<column, std::pair<T, T>> between(const matador::column &col, T low, T high)
{
  return condition<column, std::pair<T, T>>(col, std::make_pair(low, high));
}

/**
 * @brief Creates a like condition
 *
 * Creates a like condition for the given column
 * and the given value string.
 *
 * @param col The column for the like condition
 * @param val The value to the like operator
 * @return The like condition object
 */
condition<column, std::string> like(const matador::column &col, const std::string &val);
//OOS_SQL_API condition<column, std::string> like(const matador::column &col, const std::string &val);

/**
 * @brief Condition equality operator for a column and a value
 *
 * Creates a condition object of a column and a value
 * checked on equality.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the equality operation
 */
template<class T>
condition<column, T> operator==(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::EQUAL, val);
}

/**
 * @brief Condition equality method for a column and a query
 *
 * Creates a condition object of a column and a query
 * checked on equality.
 *
 * @param col The column object
 * @param q The query to compare with
 * @return The condition object representing the equality operation
 */
condition<column, detail::basic_query> equals(const column &col, detail::basic_query &q);
//OOS_SQL_API condition<column, detail::basic_query> equals(const column &col, detail::basic_query &q);

/**
 * @brief Condition inequality operator for a column and a value
 *
 * Creates a condition condition object of a column and a value
 * checked on inequality.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the inequality operation
 */
template<class T>
condition<column, T> operator!=(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::NOT_EQUAL, val);
}

/**
 * @brief Condition less operator for a column and a value
 *
 * Creates a condition object checking if the value of the given column
 * is less than the given value.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the less operation
 */
template<class T>
condition<column, T> operator<(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::LESS, val);
}

/**
 * @brief Condition less or equal operator for a column and a value
 *
 * Creates a condition object checking if the value of the given column
 * is less or equal than the given value.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the less or equal operation
 */
template<class T>
condition<column, T> operator<=(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::LESS_EQUAL, val);
}

/**
 * @brief Condition greater operator for a column and a value
 *
 * Creates a condition object checking if the value of the given column
 * is greater than the given value.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the greater operation
 */
template<class T>
condition<column, T> operator>(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::GREATER, val);
}

/**
 * @brief Condition greater or equal operator for a column and a value
 *
 * Creates a condition object checking if the value of the given column
 * is greater or equal than the given value.
 *
 * @tparam T The type of the value
 * @param col The column object
 * @param val The value to compare with
 * @return The condition object representing the greater or equal operation
 */
template<class T>
condition<column, T> operator>=(const column &col, T val)
{
  return condition<column, T>(col, detail::basic_condition::GREATER_EQUAL, val);
}

/**
 * @brief AND operation condition consisting of a left and right hand condition
 *
 * @tparam L1 Left hand type of left hand condition
 * @tparam R1 Right hand type of left hand condition
 * @tparam L2 Left hand type of right hand condition
 * @tparam R2 Right hand type of right hand condition
 * @param l Left hand condition
 * @param r Right hand condition
 * @return An condition object representing the AND operation
 */
template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator&&(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, detail::basic_condition::AND);
}

/**
 * @brief OR operation condition consisting of a left and right hand condition
 *
 * @tparam L1 Left hand type of left hand condition
 * @tparam R1 Right hand type of left hand condition
 * @tparam L2 Left hand type of right hand condition
 * @tparam R2 Right hand type of right hand condition
 * @param l Left hand condition
 * @param r Right hand condition
 * @return An condition object representing the OR operation
 */
template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator||(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, detail::basic_condition::OR);
}

/**
 * @brief Negates a given condition.
 *
 * @tparam L The left hand type of the condition
 * @tparam R The right hand type of the condition
 * @param c The condition to negated
 * @return An condition object representing the NOT operation
 */
template<class L, class R>
condition<condition<L, R>, void> operator!(const condition<L, R> &c)
{
  return condition<condition<L, R>, void>(c);
}

/**
 * @brief Creates a shared condition from a given condition object
 *
 * @tparam L The left hand type of the condition
 * @tparam R The right hand type of the condition
 * @param cond The condition to be copied
 * @return A shared condition pointer representing the given condition
 */
template<class L, class R>
std::shared_ptr<detail::basic_condition> make_condition(const condition<L, R> &cond)
{
  return std::make_shared<condition<L, R>>(cond);
}

}

#endif /* CONDITION_HPP */
