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

#ifdef _MSC_VER
  #ifdef oos_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_UTILS_API
#endif

#include <string>
#include <stdexcept>

namespace oos {

/// @cond OOS_DEV
class OOS_UTILS_API varchar_base
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

  void assign(const char *s);

  std::string str() const;

  const char* c_str() const;

  const char* data() const;

  size_type size() const;

  size_type capacity() const;

  friend OOS_UTILS_API std::ostream& operator<<(std::ostream &out, const varchar_base &val);

protected:
  void ok(const std::string &x);
  void trim();

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
  explicit varchar(const std::string &x)
    : varchar_base(C)
  {
    data_ = x;
  }

  /**
   * Initializes the varchar with the
   * given string.
   *
   * @param x The string value to set.
   */
  varchar(const char *x)
    : varchar_base(C)
  {
    data_.assign(x);
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

/**
 * Compares if two varchars are equal.
 *
 * @tparam C1 Size of left hand varchar
 * @tparam C2 Size of right hand varchar
 * @param l Left hand varchar to compare.
 * @param r Right hand varchar to compare.
 */
template < unsigned int C1, unsigned int C2 >
bool operator==(const varchar<C1> &l, const varchar<C2> &r)
{
  return l.str() == r.str();
}

/**
 * Compares if a varchar and a character array are equal.
 *
 * @tparam C Size of left hand varchar
 * @param l Left hand varchar to compare.
 * @param r Right hand character array to compare.
 */
template < unsigned int C >
bool operator==(const varchar<C> &l, const char *r)
{
  return l.str() == r;
}

/**
 * Compares if two varchars are not equal.
 *
 * @tparam C1 Size of left hand varchar
 * @tparam C2 Size of right hand varchar
 * @param l Left hand varchar to compare.
 * @param r Right hand varchar to compare.
 */
template < unsigned int C1, unsigned int C2 >
bool operator!=(const varchar<C1> &l, const varchar<C2> &r)
{
  return l.str() != r.str();
}

/**
 * Compares if a varchar and a character array are not equal.
 *
 * @tparam C Size of right hand varchar
 * @param l Left hand character array to compare.
 * @param r Right hand varchar to compare.
 */
template < unsigned int C >
bool operator!=(const varchar<C> &l, const char *r)
{
  return l.str() != r;
}

}

#endif /* VARCHAR */
