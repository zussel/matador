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
#include "sql/field.hpp"
#include "sql/token.hpp"

#include <string>
#include <sstream>
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>

namespace oos {

class basic_condition : public detail::token
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
  static std::array<std::string, num_operands> operands;
};

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
class condition<field, T, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public basic_condition
{
public:
  condition(const field &fld, basic_condition::t_operand op, T val)
    : field_(fld), operand(basic_condition::operands[op]), value(val) { }

  field field_;
  std::string operand;
  T value;

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << field_.name() << " " << operand << " " << value;
    return str.str();
  }
};

template<class T>
class condition<field, T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public basic_condition
{
public:
  condition(const field &fld, basic_condition::t_operand op, T val)
    : field_(fld), operand(basic_condition::operands[op]), value(val) { }

  field field_;
  std::string operand;
  T value;

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << field_.name() << " " << operand << " '" << value << "'";
    return str.str();
  }
};

template<class T>
class condition<T, field, typename std::enable_if<
  std::is_scalar<T>::value &&
  !std::is_same<std::string, T>::value &&
  !std::is_same<const char*, T>::value>::type> : public basic_condition
{
public:
  condition(T val, basic_condition::t_operand op, const field &fld)
    : field_(fld), operand(basic_condition::operands[op]), value(val) { }

  field field_;
  std::string operand;
  T value;

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << value << " " << operand << " " << field_.name();
    return str.str();
  }
};

template<class T>
class condition<T, field, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public basic_condition
{
public:
  condition(T val, basic_condition::t_operand op, const field &fld)
    : field_(fld), operand(basic_condition::operands[op]), value(val) { }

  field field_;
  std::string operand;
  T value;

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << "'" << value << "' " << operand << " " << field_.name();
    return str.str();
  }
};

template < class T >
class condition<field, std::initializer_list<T>, typename std::enable_if<true>::type> : public basic_condition
{
public:
  condition(const field &fld, const std::initializer_list<T> &args)
    : field_(fld), args_(args)
  {}

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << field_.name() << " IN (";
    if (args_.size() > 1) {
      auto first = args_.begin();
      auto last = args_.end() - 1;
      while (first != last) {
        str << *first++ << ",";
      }
    }
    if (!args_.empty()) {
      str << args_.back();
    }
    str << ")";
    return str.str();
  }

  field field_;
  std::vector<T> args_;
};

template < class T >
class condition<field, std::pair<T, T>, typename std::enable_if<true>::type> : public basic_condition
{
public:
  condition(const field &fld, const std::pair<T, T> &range)
    : field_(fld), range_(range) { }

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << field_.name() << " BETWEEN " << range_.first << " AND " << range_.second;
    return str.str();
  }

  field field_;
  std::pair<T, T> range_;
};

template<class L1, class R1, class L2, class R2>
class condition<condition<L1, R1>, condition<L2, R2>, typename std::enable_if<true>::type> : public basic_condition
{
public:
  condition(const condition<L1, R1> &l, const condition<L2, R2> &r, basic_condition::t_operand op)
    : left(l), right(r), operand(op) { }

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    if (operand == basic_condition::AND) {
      str << "(" << left.evaluate(prepared) << " " << basic_condition::operands[operand] << " " << right.evaluate(prepared) << ")";
    } else {
      str << left.evaluate(prepared) << " " << basic_condition::operands[operand] << " " << right.evaluate(prepared);
    }
    return str.str();
  }

  condition<L1, R1> left;
  condition<L2, R2> right;
  basic_condition::t_operand operand;
};

template<class L, class R>
class condition<condition<L, R>, void, typename std::enable_if<true>::type> : public basic_condition
{
public:
  condition(const condition<L, R> &c, basic_condition::t_operand op)
    : cond(c), operand(basic_condition::operands[op]) { }

  std::string compile(basic_dialect &d, t_compile_type)
  {
    return "";
  };

  std::string evaluate(bool prepared) const
  {
    std::stringstream str;
    str << operand << " (" << cond.evaluate(prepared) << ")";
    return str.str();
  }

  condition<L, R> cond;
  std::string operand;
};

template<class T>
condition<field, std::initializer_list<T>> in(const oos::field &f, std::initializer_list<T> args)
{
  return condition<field, std::initializer_list<T>>(f, args);
}

template<class T>
condition<field, std::pair<T, T>> between(const oos::field &f, T low, T high)
{
  return condition<field, std::pair<T, T>>(f, std::make_pair(low, high));
}

template<class T>
condition<field, T> operator==(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::EQUAL, val);
}

template<class T>
condition<field, T> operator!=(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::NOT_EQUAL, val);
}

template<class T>
condition<field, T> operator<(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::LESS, val);
}

template<class T>
condition<field, T> operator<=(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::LESS_EQUAL, val);
}

template<class T>
condition<field, T> operator>(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::GREATER, val);
}

template<class T>
condition<field, T> operator>=(const field &fld, T val)
{
  return condition<field, T>(fld, basic_condition::GREATER_EQUAL, val);
}

template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator&&(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, basic_condition::AND);
}

template<class L1, class R1, class L2, class R2>
condition<condition<L1, R1>, condition<L2, R2>> operator||(const condition<L1, R1> &l, const condition<L2, R2> &r)
{
  return condition<condition<L1, R1>, condition<L2, R2>>(l, r, basic_condition::OR);
}

template<class L, class R>
condition<condition<L, R>, void> operator!(const condition<L, R> &c)
{
  return condition<condition<L, R>, void>(c, basic_condition::NOT);
}

}

#endif /* CONDITION_HPP */
