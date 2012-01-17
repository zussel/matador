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

#ifndef VARCHAR_HPP
#define VARCHAR_HPP

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

#include <string>
#include <stdexcept>

namespace oos {

template < unsigned int CAPACITY >
class varchar
{
public:
  typedef std::string::size_type size_type;
public:
  varchar() {}

  explicit varchar(const char *str)
    : data_(str)
  {}

  explicit varchar(const std::string &str)
    : data_(str)
  {}

  varchar(const varchar &x)
    : data_(x.data_)
  {}

  varchar& operator=(const varchar &x)
  {
    data_ = x.data_;
    return *this;
  }

  varchar& operator=(const std::string &x)
  {
    data_ = x;
    return *this;
  }

  varchar& operator=(const char *x)
  {
    data_.assign(x);
    return *this;
  }

  ~varchar() {}
  
  varchar& operator+=(const varchar &x)
  {
    data_ += x.str();
    return *this;
  }

  varchar& operator+=(const std::string &x)
  {
    data_ += x;
    return *this;
  }

  varchar& operator+=(const char *x)
  {
    data_ += x;
    return *this;
  }

  operator std::string() const
  {
    return data_;
  }
  
  std::string str() const
  {
    return data_;
  }

  size_type size() const
  {
    return data_.size();
  }

  size_type capacity() const
  {
    return CAPACITY;
  }

  template < unsigned int C >
  friend std::ostream& operator<<(std::ostream &out, const varchar<C> &val) {
    out << val.str();
    return out;
  }

private:
  void ok(const std::string &x)
  {
    if (x.size() > CAPACITY) {
      throw std::logic_error("string is to long");
    }
  }

private:
  std::string data_;
};

}

#endif /* VARCHAR */
