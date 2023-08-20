#ifndef MATADOR_VARCHAR_HPP
#define MATADOR_VARCHAR_HPP

#include "is_varchar.hpp"

#include <type_traits>
#include <string>
#include <cstring>
#include <utility>

namespace matador {

struct varchar_base {};

/**
 * A varchar helper class for all has_many relations
 * where the type should be a varchar.
 *
 * Internal varchar type is of std::string
 *
 * @tparam SIZE size of the varchar
 */
template<long SIZE>
class varchar : public varchar_base
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

  explicit varchar(const char *val)
  : value_(val)
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
    size_t h2 = std::hash<long>()(SIZE);
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

  long size() const
  {
    return SIZE;
  }

private:
  value_type value_;
};

/// @cond MATADOR_DEV

/// @endcond

}
#endif //MATADOR_VARCHAR_HPP
