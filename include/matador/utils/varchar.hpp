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
  #ifdef matador_utils_EXPORTS
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

namespace matador {

/**
 * Base class for templated varchar class
 *
 * It provides the interface to work with
 * a varchar like
 * - assign
 * - operator +=
 * - trim
 *
 * Internally varchar uses a std::string to store
 * its value. One can use this class to create a
 * varchar but the templated version is easier to
 * write.
 */
class OOS_UTILS_API varchar_base
{
public:
  typedef std::string::size_type size_type; /**< Shortcut to varchars size type */

public:
  /**
   * Creates a varchar with given capacity.
   *
   * @param capacity Capacity of varchar.
   */
  explicit varchar_base(size_type capacity);

  /**
   * Creates a varchar with given std::string value and capacity
   *
   * @param x Value of the varchar
   * @param capacity Capacity of the varchar
   */
  explicit varchar_base(const std::string &x, size_type capacity);

  /**
   * Creates a varchar from given character pointer and capacity
   * @param x Value of the varchar
   * @param capacity Capacity of the varchar
   */
  explicit varchar_base(const char *x, size_type capacity);

  /**
   * Copy construct a varchar from given varchar
   *
   * @param x Varchar to copy
   */
  varchar_base(const varchar_base &x);

  /**
   * Copy move a varchar
   *
   * @param x Varchar to be moved
   */
  varchar_base(varchar_base &&x);

  /**
   * Copy assign a varchar from given varchar.
   *
   * @param x Varchar to be assigned
   * @return Reference of the new varchar
   */
  varchar_base& operator=(const varchar_base &x);

  /**
   * Assign move a varchar
   *
   * @param x Varchar to be assign moved
   * @return Reference of the new varchar
   */
  varchar_base& operator=(varchar_base &&x);

  ~varchar_base();

  /**
   * Compare varchar with the given varchar. If values
   * are equal true is returned.
   *
   * @param x Varchar to be compared
   * @return True if equal.
   */
  bool operator==(const varchar_base &x) const;

  /**
   * Compare varchar with the given varchar. If values
   * are not equal true is returned.
   *
   * @param x Varchar to be compared
   * @return True if not equal.
   */
  bool operator!=(const varchar_base &x) const;

  /**
   * Append given varchar to this varchar
   *
   * @param x Varchar to append
   * @return Reference of the modified varchar
   */
  varchar_base& operator+=(const varchar_base &x);

  /**
   * Append given std::string to this varchar
   *
   * @param x std::string to append
   * @return Reference of the modified varchar
   */
  varchar_base& operator+=(const std::string &x);

  /**
   * Append given character pointer to this varchar
   *
   * @param x Character pointer to append
   * @return Reference of the modified varchar
   */
  varchar_base& operator+=(const char *x);

  /**
   * Assign given std::string to this varchar
   *
   * @param x std::string to assign.
   */
  void assign(const std::string &x);

  /**
   * Assign given character pointer with
   * given length to this varchar
   *
   * @param s std::string to assign.
   * @param n Length of the character pointer
   */
  void assign(const char *s, size_t n);

  /**
   * Assign given character pointer to this varchar
   *
   * @param s std::string to assign.
   */
  void assign(const char *s);

  /**
   * Returns a copy of the varchar as std::string
   *
   * @return Copy of the varchar as std::string
   */
  std::string str() const;

  /**
   * Returns the underlying character pointer
   *
   * @return Underlying character pointer
   */
  const char* c_str() const;

  /**
   * Returns the underlying character pointer
   *
   * @return Underlying character pointer
   */
  const char* data() const;

  /**
   * Returns the current size of the varchar.
   *
   * @return Current size of the varchar.
   */
  size_type size() const;

  /**
   * Returns the capacity of the varchar.
   *
   * @return Capacity of the varchar.
   */
  size_type capacity() const;

  /**
   * Writes the varchar to the given std::ostream and returns the
   * reference of the modified std::ostream
   *
   * @param out The stream to write to.
   * @param val The varchar to write
   * @return Reference of the modified stream
   */
  friend OOS_UTILS_API std::ostream& operator<<(std::ostream &out, const varchar_base &val);

  /**
   * Checks if the varchar is valid e.g. current
   * size is less than or equal to capacity
   *
   * @return True if current size is less than or equal to capacity
   */
  bool valid();

  /**
   * Cuts the varchar if neccessary
   * to fit to its capacity.
   */
  void trim();

protected:
  /// @cond MATADOR_DEV
  size_type capacity_;
  std::string data_;
  /// @endcond
};

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
    : varchar_base(x.data_, C)
  {}

  /**
   * Copy move the string data
   * from the given varchar.
   *
   * @param x The varchar to copy.
   */
  varchar(varchar &&x)
    : varchar_base(std::move(x))
  {}

  /**
   * Initializes the varchar with the
   * given string.
   *
   * @param x The string value to set.
   */
  explicit varchar(const std::string &x)
    : varchar_base(x, C)
  {}

  /**
   * Initializes the varchar with the
   * given string.
   *
   * @param x The string value to set.
   */
  varchar(const char *x)
    : varchar_base(x, C)
  {}

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
   * Assignment move a varchar.
   *
   * @param x The varchar to assign.
   * @return Returns a reference to this class.
   */
  varchar& operator=(varchar &&x)
  {
    varchar_base::operator=(std::move(x));
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
    assign(x);
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
    assign(x);
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
