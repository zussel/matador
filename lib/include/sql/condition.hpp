/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONDITION_HPP
#define CONDITION_HPP

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

#include "sql/types.hpp"
#include "sql/column.hpp"
#include "sql/token.hpp"
#include "sql/basic_query.hpp"

#include <string>
#include <sstream>
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>
#include <array>

namespace oos {

namespace detail {

/// @cond OOS_DEV

class OOS_API basic_condition : public token
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
    NOT
  };

  enum
  {
    num_operands = 9
  };

  virtual void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }

  virtual std::string evaluate(basic_dialect::t_compile_type compiler_type) const = 0;

  static std::array<std::string, num_operands> operands;
};

class OOS_API basic_column_condition : public basic_condition
{
public:
  column field_;
  std::string operand;

  basic_column_condition(const column &fld, detail::basic_condition::t_operand op)
    : field_(fld), operand(detail::basic_condition::operands[op])
  { }

  virtual void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
  }
};

class OOS_API basic_in_condition : public basic_condition
{
public:
  column field_;

  basic_in_condition(const column &fld)
    : field_(fld)
  { }

  virtual void accept(token_visitor &visitor) override
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

/// @cond OOS_DEV

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

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
  {
    std::stringstream str;
    if (compile_type == basic_dialect::DIRECT) {
      str << field_.name << " " << operand << " " << value;
    } else {
      str << field_.name << " " << operand << " " << "?";
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

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
  {
    std::stringstream str;
    if (compile_type == basic_dialect::DIRECT) {
      str << field_.name << " " << operand << " '" << value << "'";
    } else {
      str << field_.name << " " << operand << " " << "?";
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

  std::string evaluate(basic_dialect::t_compile_type) const
  {
    std::stringstream str;
    str << value << " " << operand << " " << field_.name;
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

  std::string evaluate(basic_dialect::t_compile_type) const
  {
    std::stringstream str;
    str << "'" << value << "' " << operand << " " << field_.name;
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
   * @param compile_type The compile type used to evaluate
   * @return A condition IN part of the query
   */
  virtual std::string evaluate(basic_dialect::t_compile_type compile_type) const override
  {
    std::stringstream str;
    str << field_.name << " IN (";
    if (args_.size() > 1) {
      auto first = args_.begin();
      auto last = args_.end() - 1;
      while (first != last) {
        if (compile_type == basic_dialect::DIRECT) {
          str << *first++ << ",";
        } else {
          ++first;
          str << "?,";
        }
      }
    }
    if (!args_.empty()) {
      if (compile_type == basic_dialect::DIRECT) {
        str << args_.back();
      } else {
        str << "?";
      }
    }
    str << ")";
    return str.str();
  }

  /**
   * @brief Returns the number of arguments in the list
   * @return The number of arguments in the list
   */
  virtual size_t size() const override
  {
    return args_.size();
  }

private:
  std::vector<V> args_;
};

template <>
class condition<column, detail::basic_query> : public detail::basic_condition
{
public:
  condition(const column &fld, const detail::basic_query &q, basic_dialect *dialect)
          : field_(fld), query_(q), dialect_(dialect)
  {}

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
  {
    std::string result(field_.name + " IN (");
    result += dialect_->build(query_.stmt(), compile_type);
    result += (")");
    return result;
  }

  column field_;
  detail::basic_query query_;
  basic_dialect *dialect_ = nullptr;
};

template < class T >
class condition<column, std::pair<T, T>> : public detail::basic_condition
{
public:
  condition(const column &fld, const std::pair<T, T> &range)
    : field_(fld), range_(range) { }

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
  {
    std::stringstream str;
    if (compile_type == basic_dialect::DIRECT) {
      str << field_.name << " BETWEEN " << range_.first << " AND " << range_.second;
    } else {
      str << field_.name << " BETWEEN ? AND ?";
    }
    return str.str();
  }

  column field_;
  std::pair<T, T> range_;
};

template<class L1, class R1, class L2, class R2>
class condition<condition<L1, R1>, condition<L2, R2>> : public detail::basic_condition
{
public:
  condition(const condition<L1, R1> &l, const condition<L2, R2> &r, detail::basic_condition::t_operand op)
    : left(l), right(r), operand(op) { }

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
  {
    std::stringstream str;
    if (operand == detail::basic_condition::AND) {
      str << "(" << left.evaluate(compile_type) << " " << detail::basic_condition::operands[operand] << " " << right.evaluate(compile_type) << ")";
    } else {
      str << left.evaluate(compile_type) << " " << detail::basic_condition::operands[operand] << " " << right.evaluate(compile_type);
    }
    return str.str();
  }

  condition<L1, R1> left;
  condition<L2, R2> right;
  detail::basic_condition::t_operand operand;
};

template<class L, class R>
class condition<condition<L, R>, void> : public detail::basic_condition
{
public:
  condition(const condition<L, R> &c, detail::basic_condition::t_operand op)
    : cond(c), operand(detail::basic_condition::operands[op]) { }

  std::string evaluate(basic_dialect::t_compile_type compiler_type) const
  {
    std::stringstream str;
    str << operand << " (" << cond.evaluate(compiler_type) << ")";
    return str.str();
  }

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
condition<column, std::initializer_list<V>> in(const oos::column &col, std::initializer_list<V> args)
{
  return condition<column, std::initializer_list<V>>(col, args);
}

/**
 * @brief Creates an IN condition for a given column and a query to be executed
 *
 * @param col The column to compare
 * @param q The query to be executes as sub select
 * @param dialect A pointer to the sql dialect
 * @return The condition object
 */
OOS_API condition<column, detail::basic_query> in(const oos::column &col, detail::basic_query &q, basic_dialect *dialect);

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
condition<column, std::pair<T, T>> between(const oos::column &col, T low, T high)
{
  return condition<column, std::pair<T, T>>(col, std::make_pair(low, high));
}

template<class T>
condition<column, T> operator==(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::EQUAL, val);
}

template<class T>
condition<column, T> operator!=(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::NOT_EQUAL, val);
}

template<class T>
condition<column, T> operator<(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::LESS, val);
}

template<class T>
condition<column, T> operator<=(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::LESS_EQUAL, val);
}

template<class T>
condition<column, T> operator>(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::GREATER, val);
}

template<class T>
condition<column, T> operator>=(const column &fld, T val)
{
  return condition<column, T>(fld, detail::basic_condition::GREATER_EQUAL, val);
}

template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator&&(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, detail::basic_condition::AND);
}

template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator||(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, detail::basic_condition::OR);
}

template<class L, class R>
condition<condition<L, R>, void> operator!(const condition<L, R> &c)
{
  return condition<condition<L, R>, void>(c, detail::basic_condition::NOT);
}

template<class L, class R>
std::shared_ptr<detail::basic_condition> make_condition(const condition<L, R> &cond)
{
  return std::make_shared<condition<L, R>>(cond);
}

}

#endif /* CONDITION_HPP */
