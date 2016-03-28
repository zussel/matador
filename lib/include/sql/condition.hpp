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

#include <string>
#include <sstream>
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>

namespace oos {

namespace detail {

class basic_condition : public token
{
public:
  basic_condition() : token(basic_dialect::CONDITION) { }

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

  std::string compile(basic_dialect &d) const
  {
    return evaluate(d.compile_type());
  };

  virtual std::string evaluate(basic_dialect::t_compile_type compiler_type) const = 0;

  static std::array<std::string, num_operands> operands;
};

}

/**
 * @class condition
 * @brief Represents a sql query condition
 * 
 * This class represents a condition part
 * of a sql query or update statement.
 * Each compare method returns a reference to
 * the condition itself. That way one can
 * concatenate conditions together.
 */
template<class L, class R, class Enabled = void>
class condition;

template<class T>
class condition<column, T, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public detail::basic_condition
{
public:
  condition(const column &fld, detail::basic_condition::t_operand op, T val)
    : field_(fld), operand(detail::basic_condition::operands[op]), value(val) { }

  column field_;
  std::string operand;
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
  std::is_same<const char*, T>::value>::type> : public detail::basic_condition
{
public:
  condition(const column &fld, detail::basic_condition::t_operand op, T val)
    : field_(fld), operand(detail::basic_condition::operands[op]), value(val) { }

  column field_;
  std::string operand;
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
  !std::is_same<const char*, T>::value>::type> : public detail::basic_condition
{
public:
  condition(T val, detail::basic_condition::t_operand op, const column &fld)
    : field_(fld), operand(detail::basic_condition::operands[op]), value(val) { }

  column field_;
  std::string operand;
  T value;

  std::string evaluate(basic_dialect::t_compile_type compiler_type) const
  {
    std::stringstream str;
    str << value << " " << operand << " " << field_.name;
    return str.str();
  }
};

template<class T>
class condition<T, column, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public detail::basic_condition
{
public:
  condition(T val, detail::basic_condition::t_operand op, const column &fld)
    : field_(fld), operand(detail::basic_condition::operands[op]), value(val) { }

  column field_;
  std::string operand;
  T value;

  std::string evaluate(basic_dialect::t_compile_type) const
  {
    std::stringstream str;
    str << "'" << value << "' " << operand << " " << field_.name;
    return str.str();
  }
};

template < class T >
class condition<column, std::initializer_list<T>, typename std::enable_if<true>::type> : public detail::basic_condition
{
public:
  condition(const column &fld, const std::initializer_list<T> &args)
    : field_(fld), args_(args)
  {}

  std::string evaluate(basic_dialect::t_compile_type compile_type) const
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

  column field_;
  std::vector<T> args_;
};

template < class T >
class condition<column, std::pair<T, T>, typename std::enable_if<true>::type> : public detail::basic_condition
{
public:
  condition(const column &fld, const std::pair<T, T> &range)
    : field_(fld), range_(range) { }

  std::string evaluate(basic_dialect::t_compile_type) const
  {
    std::stringstream str;
    str << field_.name << " BETWEEN " << range_.first << " AND " << range_.second;
    return str.str();
  }

  column field_;
  std::pair<T, T> range_;
};

template<class L1, class R1, class L2, class R2>
class condition<condition<L1, R1>, condition<L2, R2>, typename std::enable_if<true>::type> : public detail::basic_condition
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
class condition<condition<L, R>, void, typename std::enable_if<true>::type> : public detail::basic_condition
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

template<class T>
condition<column, std::initializer_list<T>> in(const oos::column &f, std::initializer_list<T> args)
{
  return condition<column, std::initializer_list<T>>(f, args);
}

template<class T>
condition<column, std::pair<T, T>> between(const oos::column &f, T low, T high)
{
  return condition<column, std::pair<T, T>>(f, std::make_pair(low, high));
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

}

#endif /* CONDITION_HPP */
