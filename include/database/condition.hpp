#ifndef CONDITION_HPP
#define CONDITION_HPP

#include "database/sql.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <memory>

namespace oos {

class condition
{
public:
  condition()
    : valid_(false)
    , next_(0)
  {}
  condition(const std::string &c)
    : column_(c)
    , valid_(false)
    , next_(0)
  {}
  /*
  condition(const condition &x)
    : column_(x.column_)
    , type_(x.type_)
    , size_(x.size_)
    , value_(x.value_)
    , op_(x.op_)
    , logic_(x.logic_)
    , valid_(x.valid_)
    , next_(x.next_)
  {}
  */
  ~condition()
  {}
  
  template < class T >
  condition& equal(const T &val)
  {
    set(val, "=");
    return *this;
  }

  template < class T >
  condition& not_equal(const T &val)
  {
    set(val, "!=");
    return *this;
  }

  template < class T >
  condition& greater(const T &val)
  {
    set(val, ">");
    return *this;
  }

  template < class T >
  condition& greater_equal(const T &val)
  {
    set(val, ">=");
    return *this;
  }

  template < class T >
  condition& less(const T &val)
  {
    set(val, "<");
    return *this;
  }

  template < class T >
  condition& less_equal(const T &val)
  {
    set(val, "<=");
    return *this;
  }

  condition& not_null()
  {
    op_ = " IS NOT NULL";
    valid_ = true;
    return *this;
  }

  condition& null()
  {
    op_ = " IS NULL";
    valid_ = true;
    return *this;
  }

  condition& or_(const condition &cond);
  condition& and_(const condition &cond);
  /*
  condition& not_(const condition &cond);
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
  
  std::string column() const
  {
    return column_;
  }
  
  unsigned long size() const
  {
    return size_;
  }
  
  sql::data_type_t type() const
  {
    return type_;
  }

  bool valid() const
  {
    return valid_;
  }

  std::ostream& print(std::ostream &out, bool prepared) const;

protected:
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

private:
  std::string column_;
  sql::data_type_t type_;
  unsigned long size_;
  std::string value_;
  std::string op_;
  std::string logic_;
  bool valid_;
  std::shared_ptr<condition> next_;
};

condition cond(const std::string &c);

}

#endif /* CONDITION_HPP */
