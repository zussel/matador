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

#ifdef WIN32
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

#include "database/types.hpp"

#include <string>
#include <sstream>

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

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
class OOS_API condition
{
public:
  /**
   * Creates an empty unknown
   * condition.
   */
  condition()
    : valid_(false)
  {}
  /**
   * Creates a new condition for
   * the given column c
   * 
   * @param c The column name.
   */
  condition(const std::string &c)
    : column_(c)
    , valid_(false)
  {}

  ~condition()
  {}

  /**
   * Evalutes the value of the column
   * with the given value to equal.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& equal(const T &val)
  {
    set(val, "=");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * with the given value to not equal.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& not_equal(const T &val)
  {
    set(val, "!=");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * with the given value to greater than.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& greater(const T &val)
  {
    set(val, ">");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * with the given value to equal or
   * greater than.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& greater_equal(const T &val)
  {
    set(val, ">=");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * with the given value to less than.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& less(const T &val)
  {
    set(val, "<");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * with the given value to equal or
   * less than.
   * 
   * @tparam T The type of the value.
   * @param val The value to compares
   * @return A reference to the condition.
   */
  template < class T >
  condition& less_equal(const T &val)
  {
    set(val, "<=");
    return *this;
  }

  /**
   * Evalutes the value of the column
   * to not null.
   * 
   * @return A reference to the condition.
   */
  condition& not_null()
  {
    op_ = " IS NOT NULL";
    valid_ = true;
    return *this;
  }

  /**
   * Evalutes the value of the column
   * to null.
   * 
   * @return A reference to the condition.
   */
  condition& null()
  {
    op_ = " IS NULL";
    valid_ = true;
    return *this;
  }

  /**
   * Concatenate to conditions with logical or.
   * 
   * @return A reference to the condition.
   */
  condition& or_(const condition &cond);

  /**
   * Concatenate to conditions with logical and.
   * 
   * @return A reference to the condition.
   */
  condition& and_(const condition &cond);
  /*
  condition& not_(const condition &cond);
  */

  /**
   * Returns the string representation
   * of the condition. With the prepared
   * flag set to true the condition string
   * is put together for preppared
   * statements.
   * 
   * @param prepared Indicates wether the condition is
   *        for prepared statements or not.
   * @return A reference to the condition.
   */
  std::string str(bool prepared) const
  {
    std::stringstream c;
    if (next_) {
      c << "(";
    } else {
      c << " ";
    }
    print(c, prepared);
    if (next_) {
      c << ")";
    }
    return c.str();
  }
  
  /**
   * Returns the column name.
   * 
   * @return Returns the column name.
   */
  std::string column() const
  {
    return column_;
  }
  
  /**
   * Returns the size of the data
   * 
   * @return The size of the data.
   */
  unsigned long size() const
  {
    return size_;
  }
  
  /**
   * Returns the data type.
   * 
   * @return The data type.
   */
  data_type_t type() const
  {
    return type_;
  }

  /**
   * Returns wether the condition
   * is in a valid state. This means
   * a column name, an operator and a
   * value must be set.
   * 
   * @return True if the condition is in a valid state.
   */
  bool valid() const
  {
    return valid_;
  }

protected:

/// @cond OOS_DEV
  std::ostream& print(std::ostream &out, bool prepared) const;

  template < class T >
  void set(const T &val, const char *op)
  {
    op_ = op;
    type_ = type_traits<T>::data_type();
    size_ = type_traits<T>::type_size();
    value(val);
    valid_ = true;
  }
  void set(const char *val, const char *op)
  {
    op_ = op;
    type_ = type_traits<const char*>::data_type();
    size_ = type_traits<const char*>::type_size();
    value(std::string(val));
    valid_ = true;
  }

  template < class T >
  void value(const T &val)
  {
    std::stringstream msg;
    msg << val;
    value_ = msg.str();
  }
  
  void value(const std::string &val)
  {
    std::stringstream msg;
    msg << "'" << val << "'";
    value_ = msg.str();
  }
/// @endcond

private:
  std::string column_;
  data_type_t type_;
  unsigned long size_;
  std::string value_;
  std::string op_;
  std::string logic_;
  bool valid_;
  std::tr1::shared_ptr<condition> next_;
};

/**
 * Creates a condition for the given
 * column name.
 * 
 * @param column The name of the column.
 * @return A new condition.
 */
OOS_API condition cond(const std::string &column);

}

#endif /* CONDITION_HPP */
