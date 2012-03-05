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

/// @cond OOS_DEV
class OOS_API varchar_base
{
public:
  typedef std::string::size_type size_type;

public:
  explicit varchar_base(size_type capacity);

  varchar_base(const varchar_base &x);

  varchar_base& operator=(const varchar_base &x);

  varchar_base& operator=(const std::string &x);

  varchar_base& operator=(const char *x);

  ~varchar_base();
  
  bool operator==(const varchar_base &x) const;

  bool operator!=(const varchar_base &x) const;

  varchar_base& operator+=(const varchar_base &x);

  varchar_base& operator+=(const std::string &x);

  varchar_base& operator+=(const char *x);

  void assign(const char *s, size_t n);

  std::string str() const;

  const char* c_str() const;

  size_type size() const;

  size_type capacity() const;

  friend OOS_API std::ostream& operator<<(std::ostream &out, const varchar_base &val);

protected:
  void ok(const std::string &x);

protected:
  size_type capacity_;
  std::string data_;
};
/// @endcond

/**
 * @tparam C The capacity of the varchar.
 * @class varchar
 * @brief A simple varchar class.
 *
 * This class provides string class with the 
 * SQL VARCHAR type in mind. The capacity of
 * the string is given within the template
 * parameter of type unsigned int.
 * The real string is represented internaly
 * by a std::string.
 */
template < unsigned int C >
class varchar : public varchar_base
{
public:
  /**
   * Creates an empty varchar
   * with the given capacity
   */
  varchar()
    : varchar_base(C)
  {}

  /**
   * Copies the string data
   * from the given varchar.
   *
   * @param x The varchar to copy.
   */
  varchar(const varchar &x)
    : varchar_base(C)
  {
    data_ = x.data_;
  }

  /**
   * Initializes the varchar with the
   * given string.
   *
   * @param x The string value to set.
   */
  varchar(const std::string &x)
    : varchar_base(C)
  {
    data_ = x;
  }

  /**
   * Assigns a varchar.
   *
   * @param x The varchar to assign.
   * @return Returns a reference to this class.
   */
  varchar& operator=(const varchar &x)
  {
    
    capacity_ = C;
    data_ = x.data_;
    return *this;
  }

  /**
   * Assigns a string to the varchar
   *
   * @param x The string to assign.
   * @return Returns a reference to this class.
   */
  varchar& operator=(const std::string &x)
  {
    data_ = x;
    return *this;
  }

  /**
   * Assigns a character array to the varchar
   *
   * @param x The character array to assign.
   * @return Returns a reference to this class.
   */
  varchar& operator=(const char *x)
  {
    varchar_base::operator=(x);
    return *this;
  }
};

template < unsigned int C >
bool operator==(const varchar<C> &l, const varchar<C> &r)
{
  return l.c_str() == r.c_str();
}

template < unsigned int C >
bool operator!=(const varchar<C> &l, const varchar<C> &r)
{
  return l.c_str() != r.c_str();
}


}

#endif /* VARCHAR */
