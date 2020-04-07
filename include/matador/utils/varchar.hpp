//
// Created by sascha on 17.09.19.
//

#ifndef MATADOR_VARCHAR_HPP
#define MATADOR_VARCHAR_HPP

#include "is_varchar.hpp"

#include <type_traits>
#include <string>
#include <cstring>
#include <utility>

namespace matador {

struct varchar_base {};

template<int SIZE, class T = std::string>
class varchar;

/**
 * A varchar helper class for all has_many relations
 * where the type should be a varchar
 *
 * Default varchar type is of std::string, but const char*
 * and char[] is also valid
 *
 * @tparam SIZE size of the varchar
 */
template<int SIZE>
class varchar<SIZE, std::string> : public varchar_base
{
public:
  typedef std::string value_type; /**< Shortcut of the internal varchar type */

  /**
   * Creates an empty varchar
   */
  varchar() = default;
  /**
   * Creates a varchar with given value
   *
   * @param val Value of varchar
   */
  explicit varchar(value_type val)
  : value_(std::move(val))
  {}

  /**
   * Provides the less to compare operator
   *
   * @param x Value to compare
   * @return True if this is less than x
   */
  bool operator<(const varchar &x) const
  {
    return value_ < x.value_;
  }

  /**
   * Provides the equal to operator
   *
   * @param x Value to compare
   * @return True if values are equal
   */
  bool operator==(const varchar &x) const
  {
    return value_ == x.value_;
  }

  /**
   * Provides the not equal to operator
   *
   * @param x Value to compare
   * @return True if values are not equal
   */
  bool operator!=(const varchar &x) const
  {
    return !operator==(x);
  }

  /**
   * Assigns a new std::string value to varchar
   *
   * @param val Value to assign
   */
  void assign(const std::string &val)
  {
    value_ = val;
  }

  /**
   * Assigns a new char value to varchar
   *
   * @param val Value to assign
   */
  void assign(const char *val)
  {
    value_.assign(val);
  }

  /**
   * Creates a hash for storing in a hash map/set
   *
   * @return A hash based on current value and size
   */
  size_t hash() const
  {
    size_t h1 = std::hash<std::string>()(value_);
    size_t h2 = std::hash<int>()(size);
    return h1 ^ (h2 << 1);
  }

  /**
   * Returns true if varchar is empty
   *
   * @return True if empty
   */
  bool empty() const
  {
    return value_.empty();
  }

  /**
   * Return a reference to the internal value
   *
   * @return Reference to the internal value
   */
  value_type& value() {
    return value_;
  }

  /**
   * Return a const reference to the internal value
   *
   * @return Const reference to the internal value
   */
  const value_type& value() const {
    return value_;
  }

private:
  int size = SIZE;
  value_type value_;
};

/// @cond MATADOR_DEV

template<int SIZE>
class varchar<SIZE, char> : public varchar_base
{
public:
  typedef char value_type;

  varchar()
  {
    value_[0] = '\0';
  };

  explicit varchar(const value_type *val) {
    assign(val);
  }

  bool operator<(const varchar &x) const
  {
    return strcmp(value_, x.value) < 0;
  }

  bool operator==(const varchar &x) const
  {
    return strcmp(value_, x.value) == 0;
  }

  bool operator!=(const varchar &x) const
  {
    return !operator==(x);
  }

  void assign(const std::string &val)
  {
    assign(val.c_str());
  }

  void assign(const char *val)
  {
    auto len = strlen(val);
    len = len > size ? size : len;
#ifdef _MSC_VER
    strncpy_s(value, size, val, len);
#else
    strncpy(value_, val, len);
#endif
    value_[len] = '\0';
  }

  size_t hash() const
  {
    size_t h1 = std::hash<char>()(value_);
    size_t h2 = std::hash<int>()(size);
    return h1 ^ (h2 << 1);
  }

  bool empty() const
  {
    return strlen(value_) == 0;
  }

  value_type& value() {
    return value_;
  }

  const value_type& value() const {
    return value_;
  }

private:
  int size = SIZE;
  value_type value_[SIZE] = {};
};

template<int SIZE>
class varchar<SIZE, char*> : public varchar_base
{
public:
  typedef char* value_type;

  varchar() : value_(new char[SIZE]) {}

  explicit varchar(value_type val)
  {
    assign(val);
  }

  ~varchar()
  {
    delete [] value_;
  }

  bool operator<(const varchar &x) const
  {
    return strcmp(value_, x.value()) < 0;
  }

  bool operator==(const varchar &x) const
  {
    return strcmp(value_, x.value()) == 0;
  }

  bool operator!=(const varchar &x) const
  {
    return !operator==(x);
  }

  void assign(const std::string &val)
  {
    assign(val.c_str());
  }

  void assign(const char *val)
  {
    auto len = strlen(val);
    len = len > (size_t)size ? (size_t)size : len;
#ifdef _MSC_VER
    strncpy_s(value, size, val, len);
#else
    strncpy(value_, val, len);
#endif
    value_[len] = '\0';
  }

  size_t hash() const
  {
    size_t h1 = std::hash<const char*>()(value_);
    size_t h2 = std::hash<int>()(size);
    return h1 ^ (h2 << 1);
  }

  bool empty() const
  {
    return value_ == nullptr || strlen(value_) == 0;
  }

  char* value() {
    return value_;
  }

  const char* value() const {
    return value_;
  }

private:
  int size = SIZE;
  value_type value_ = nullptr;
};

/// @endcond

}
#endif //MATADOR_VARCHAR_HPP
