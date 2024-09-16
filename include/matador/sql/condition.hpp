#ifndef QUERY_CONDITION_HPP
#define QUERY_CONDITION_HPP

#include "matador/sql/basic_condition.hpp"
#include "matador/sql/dialect.hpp"
#include "matador/sql/query_context.hpp"

#include "matador/utils/placeholder.hpp"

#include <memory>
#include <utility>

namespace matador::sql {

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

template<>
class condition<column, utils::placeholder, std::enable_if_t<true>> final : public basic_column_condition
{
public:
    condition(const column &fld, basic_condition::operand_t op, const utils::placeholder &val);

    utils::placeholder value;

    std::string evaluate(const dialect &d, query_context &query) const override;
};

template<class T>
class condition<column, T, typename std::enable_if<
  std::is_scalar_v<T> &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public basic_column_condition
{
public:
  condition(const column &fld, basic_condition::operand_t op, T val)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(const dialect &d, query_context &query) const override
  {
    query.bind_vars.emplace_back(field_.name);
    return d.prepare_condition(field_) + " " + operand + " " + std::to_string(value);
  }
};

template<class T>
class condition<column, T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public basic_column_condition
{
public:
  condition(const column &fld, basic_condition::operand_t op, T val)
    : basic_column_condition(fld, op)
    ,value(val)
  { }

  T value;

  std::string evaluate(const dialect &d, query_context &query) const override
  {
    query.bind_vars.emplace_back(field_.name);
    return d.prepare_identifier(field_) + " " + operand + " '" + value + "'";
  }
};

template<class T>
class condition<T, column, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public basic_column_condition
{
public:
  condition(T val, basic_condition::operand_t op, const column &fld)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(const dialect &d, query_context &query) const override
  {
    return std::to_string(value) + " " + operand + " " + d.prepare_identifier(field_);
  }
};

template<class T>
class condition<T, column, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public basic_column_condition
{
public:
  condition(T val, basic_condition::operand_t op, const column &fld)
    : basic_column_condition(fld, op)
    , value(val)
  { }

  T value;

  std::string evaluate(const dialect &d, query_context &query) const override
  {
    return "'" + std::to_string(value) + "' " + operand + " " + d.prepare_identifier(field_);
  }
};

/// @endcond

/**
 * @brief Condition class representing an IN condition
 *
 * This class represents an query IN condition and evaluates to
 * this condition based on the current database d
 *
 * @code
 * WHERE age IN (29,34,56)
 * @endcode
 */
template < class V >
class condition<column, std::initializer_list<V>> : public basic_in_condition {
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
  : basic_in_condition(col), args_(args) {}

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a part of the
   * query string based on the given compile type
   *
   * @param d The d used to evaluate
   * @return A condition IN part of the query
   */
  std::string evaluate(const dialect &d, query_context &query) const override {
    auto count = size();
    for (size_t i = 0; i < count; ++i) {
      query.bind_vars.emplace_back(field_.name);
    }

    std::string result = d.prepare_identifier(field_) + " IN (";
    if (args_.size() < 2) {
      for (const auto &val : args_) {
        result.append(std::to_string(val));
      }
    } else {
      auto it = args_.begin();
      result.append(std::to_string(*it++));
      for(; it != args_.end(); ++it) {
        result.append(", " + std::to_string(*it));
      }
    }
    result += ")";
    return result;
  }

  /**
   * @brief Returns the number of arguments in the list
   * @return The number of arguments in the list
   */
  [[nodiscard]] size_t size() const override {
    return args_.size();
  }

private:
  std::vector<V> args_;
};

/**
 * @brief Condition class representing an IN condition
 *
 * This class represents an query IN condition and evaluates to
 * this condition based on the current database d
 *
 * @code
 * WHERE age IN (select age_value from <table>)
 * @endcode
 */
template <>
class condition<column, query_context> : public basic_column_condition
{
public:
  /**
   * @brief Create a query IN condition
   *
   * Create an IN condition where the argument values come from
   * the given query. To evaluate the query a sql d must be
   * given.
   *
   * @param col Column for the IN condition
   * @param op Operand of the condition
   * @param q The query to be evaluated to the IN arguments
   */
  condition(column col, basic_condition::operand_t op, query_context &q);

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a part of the
   * query string based on the given compile type
   *
   * @param d The d used to evaluate
   * @return A condition IN part of the query
   */
  std::string evaluate(const dialect &d, query_context &query) const override;

private:
  query_context &query_;
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
class condition<column, std::pair<T, T>> : public basic_condition {
public:
  /**
   * @brief Create a new between condition
   *
   * @param col The column for the range check
   * @param range The boundary values defining the range
   */
  condition(column col, const std::pair<T, T> &range)
  : field_(std::move(col)), range_(range) {}

  /**
   * @brief Evaluates the condition
   *
   * Evaluates the condition to a between part
   * based on the given compile type
   *
   * @param d The d used to evaluate
   * @return A condition BETWEEN part of the query
   */
  std::string evaluate(const dialect &d, query_context &query) const override {
    query.bind_vars.emplace_back(field_.name);
    query.bind_vars.emplace_back(field_.name);
    return d.prepare_identifier(field_) + " BETWEEN " + std::to_string(range_.first) + " AND " + std::to_string(range_.second);
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
class condition<condition<L1, R1>, condition<L2, R2>> : public basic_condition
{
public:
  /**
   * @brief Create a binary logical condition
   * @param l Left hand operator of the condition
   * @param r right hand operator of the condition
   * @param op The operand (AND or OR)
   */
  condition(condition<L1, R1> &&l, condition<L2, R2> &&r, basic_condition::operand_t op)
    : left(std::move(l)), right(std::move(r)), operand(op) { }

  /**
   * @brief Evaluates the condition
   *
   * @param d The d used to evaluate
   * @return The evaluated string based on the compile type
   */
  std::string evaluate(const dialect &d, query_context &query) const override
  {
    // ensure the numbering order for host vars
    auto cl = left.evaluate(d, query);
    auto cr = right.evaluate(d, query);
    if (operand == basic_condition::operand_t::AND) {
      return "(" + cl + " " + basic_condition::operands[operand] + " " + cr + ")";
    } else {
      return cl + " " + basic_condition::operands[operand] + " " + cr;
    }
  }

private:
  condition<L1, R1> left;
  condition<L2, R2> right;
  basic_condition::operand_t operand;
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
class condition<condition<L, R>, void> final : public basic_condition
{
public:
  /**
   * @brief Create a logical unary condition
   * @param c The condition to be negated
   */
  condition(const condition<L, R> &c) // NOLINT(*-explicit-constructor)
    : cond(c), operand(basic_condition::operands[basic_condition::operand_t::NOT]) { }

  /**
   * @brief Evaluates the condition
   *
   * @param d The d used to evaluate
   * @param query The context of the query
   * @return The evaluated string based on the compile type
   */
  std::string evaluate(const dialect &d, query_context &query) const override
  {
    return operand + " (" + cond.evaluate(d, query) + ")";
  }

private:
  condition<L, R> cond;
  std::string operand;
};

template<>
class condition<column, column> final : public basic_column_condition
{
public:
  condition(const column &a, const basic_condition::operand_t op, column b)
  : basic_column_condition(a, op)
  , other_column_(std::move(b)) {}
  /**
   * @brief Evaluates the condition
   *
   * @param d The d used to evaluate
   * @param query The context of the query
   * @return The evaluated string based on the compile type
   */
  std::string evaluate(const dialect &d, query_context &query) const override;

private:
  column other_column_;
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
condition<column, std::initializer_list<V>> in(const column &col, std::initializer_list<V> args)
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
condition<column, query_context> in(const column &col, query_context &&q);

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
condition<column, std::pair<T, T>> between(const column &col, T low, T high)
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
condition<column, std::string> like(const column &col, const std::string &val);

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
  return condition<column, T>(col, basic_condition::operand_t::EQUAL, val);
}

condition<column, column> operator==(const column &a, const column &b);

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
condition<column, query_context> equals(const column &col, query_context &q);

/**
 * @brief Condition inequality operator for a column and a value
 *
 * Creates a condition object of a column and a value
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
  return condition<column, T>(col, basic_condition::operand_t::NOT_EQUAL, val);
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
  return condition<column, T>(col, basic_condition::operand_t::LESS, val);
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
  return condition<column, T>(col, basic_condition::operand_t::LESS_EQUAL, val);
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
  return condition<column, T>(col, basic_condition::operand_t::GREATER, val);
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
  return condition<column, T>(col, basic_condition::operand_t::GREATER_EQUAL, val);
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
condition<condition<L1, R1>, condition<L2, R2>> operator&&(condition<L1, R1> l, condition<L2, R2> r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(std::move(l), std::move(r), basic_condition::operand_t::AND);
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
condition<condition<L1, R1>, condition<L2, R2>> operator||(condition<L1, R1> l, condition<L2, R2> r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(std::move(l), std::move(r), basic_condition::operand_t::OR);
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
condition<condition<L, R>, void> operator!(condition<L, R> c)
{
  return condition<condition<L, R>, void>(std::move(c));
}

/**
 * @brief Creates a unique condition from a given condition object
 *
 * @tparam L The left hand type of the condition
 * @tparam R The right hand type of the condition
 * @param cond The condition to be copied
 * @return A unique condition pointer representing the given condition
 */
template<class L, class R>
std::unique_ptr<basic_condition> make_condition(const condition<L, R> &cond)
{
  return std::make_unique<condition<L, R>>(cond);
}

}

#endif //QUERY_CONDITION_HPP
