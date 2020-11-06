#ifndef MATADOR_OPTIONAL_HPP
#define MATADOR_OPTIONAL_HPP

#include <stdexcept>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

struct nullopt_t
{
  struct init{};
  explicit nullopt_t( init ) {}
};

/// @endcond

}

/**
 * Null value for optionals
 */
const detail::nullopt_t nullopt(( detail::nullopt_t::init() ));

/**
 * Excpetion class for optional
 */
class bad_optional_access : public std::logic_error
{
public:
  /**
   * Default constructor
   */
  explicit bad_optional_access()
    : logic_error( "bad optional access" ) {}
};

/**
 * The optional class provides an object
 * holding a value of type T or not. The
 * value is optional.
 *
 * @tparam T Type of the hold optional value
 */
template < typename T >
class optional
{
public:
  typedef T value_type; /**< Shortcut to the value type */

  /**
   * Default constructor
   */
  optional() = default;

  /**
   * Initializes the optional with a null
   * type.
   */
  optional(detail::nullopt_t) : has_value_(false) { };

  /**
   * Initializes a new optional with
   * the given value.
   *
   * @param value Value of optional
   */
  optional(const T &value)
    : has_value_(true), value_(value)
  {}

  /**
   * Copies given optional value x of type U to
   * new optional value of type T. A value of
   * type U must be convertible to type T.
   *
   * @tparam U Type of other optional
   * @param x Other optional value
   */
  template< class U >
  optional(const optional<U>& x)
    : has_value_(x.has_value()) , value_(x.value())
  {}

  /**
   * Assigns a null optional value to this
   * optional. Optional will be set to nullopt.
   *
   * @return The assigned optional
   */
  optional& operator=(detail::nullopt_t)
  {
    reset();
    return *this;
  }

  /**
   * Assigns an other optional of type U to this
   * optional. Type U must be convertible to
   * type T.
   *
   * @tparam U Type of other optional
   * @param x Other optional to be assigned
   * @return The assigned optional
   */
  template< class U >
  optional& operator=(const optional<U>& x)
  {
    has_value_ = x.has_value();
    value_ = x.value();
    return *this;
  }

  /**
   * Access the underlying value as pointer.
   * The value is checked on validity with assert
   *
   * @return The value as pointer
   */
  const value_type* operator->() const
  {
    return assert( has_value() ), &value_;
  }

  /**
   * Access the underlying value as pointer.
   * The value is checked on validity with assert
   *
   * @return The value as pointer
   */
  value_type* operator->()
  {
    return assert( has_value() ), &value_;
  }

  /**
   * Access the underlying value as reference.
   * The value is checked on validity with assert
   *
   * @return The value as reference
   */
  const value_type& operator*() const
  {
    return assert( has_value() ), value_;
  }

  /**
   * Access the underlying value as reference.
   * The value is checked on validity with assert
   *
   * @return The value as reference
   */
  value_type& operator*()
  {
    return assert( has_value() ), value_;
  }

  /**
   * Boolean operator returns true if
   * optional has a value
   *
   * @return True if valueis available
   */
  explicit operator bool() const
  {
    return has_value();
  }

  /**
   * Returns a const reference to the value.
   * If value isn't available a bad_optional_exception
   * is thrown
   *
   * @throws bad_optional_exception If not available
   * @return Const reference to the value
   */
  const value_type& value() const
  {
    if (!has_value()) {
      throw bad_optional_access();
    }
    return value_;
  }

  /**
   * Returns a reference to the value.
   * If value isn't available a bad_optional_exception
   * is thrown
   *
   * @throws bad_optional_exception If not available
   * @return Reference to the value
   */
  value_type& value()
  {
    if (!has_value()) {
      throw bad_optional_access();
    }
    return value_;
  }

  /**
   * Return a copy of the value or in case
   * value is not set return the given value
   *
   * This default value is of type U and must be
   * convertible to type T.
   *
   * @tparam U Type of default value
   * @param v Default value if optional value is not set
   * @return Optional value or default value
   */
  template< class U >
  value_type value_or( U const & v ) const
  {
    return has_value() ? value() : static_cast<value_type>( v );
  }

  /**
   * Returns true if value is set.
   *
   * @return True if value is set
   */
  bool has_value() const
  {
    return has_value_;
  }

  /**
   * Resets optional to empty (no value set)
   */
  void reset()
  {
    has_value_ = false;
  }

private:
  bool has_value_ = false;
  T value_;
};

/**
 * Equal compare operator for optionals.
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if optional are equal
 */
template< typename T, typename U >
inline bool operator==( optional<T> const & x, optional<U> const & y )
{
  return bool(x) == bool(y) && (!bool(x) || *x == *y);
}

/**
 * Not equal compare operator for optionals.
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if optional are not equal
 */
template< typename T, typename U >
inline bool operator!=( optional<T> const & x, optional<U> const & y )
{
  return !(x == y);
}

/**
 * Less operator for optionals
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if left optional is less than right optional
 */
template< typename T, typename U >
inline bool operator<( optional<T> const & x, optional<U> const & y )
{
  return y && ((!x) || *x < *y);
}

/**
 * Greater operator for optionals
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if left optional is greater than right optional
 */
template< typename T, typename U >
inline bool operator>( optional<T> const & x, optional<U> const & y )
{
  return (y < x);
}

/**
 * Less equal operator for optionals
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if left optional is less equal than right optional
 */
template< typename T, typename U >
inline bool operator<=( optional<T> const & x, optional<U> const & y )
{
  return !(y < x);
}

/**
 * Greater equal operator for optionals
 *
 * @tparam T Type of left optional
 * @tparam U Type of right optional
 * @param x Left optional to compare
 * @param y Right optional to compare
 * @return True if left optional is greater equal than right optional
 */
template< typename T, typename U >
inline bool operator>=( optional<T> const & x, optional<U> const & y )
{
  return !(x < y);
}

/**
 * Equal to null value for optionals
 *
 * @tparam T Type of left optional
 * @param x Left optional to compare
 * @return True if optional is null
 */
template< typename T>
inline bool operator==(optional<T> const &x, detail::nullopt_t)
{
  return (!x);
}

/**
 * Equal to null value for optionals
 *
 * @tparam T Type of right optional
 * @param x Right optional to compare
 * @return True if optional is null
 */
template< typename T>
inline bool operator==(detail::nullopt_t, optional<T> const &x)
{
  return (!x);
}

/**
 * Not equal to null value for optionals
 *
 * @tparam T Type of left optional
 * @param x Left optional to compare
 * @return True if optional is not null
 */
template< typename T>
inline bool operator!=(optional<T> const &x, detail::nullopt_t)
{
  return bool(x);
}

/**
 * Not equal to null value for optionals
 *
 * @tparam T Type of right optional
 * @param x Right optional to compare
 * @return True if optional is not null
 */
template< typename T>
inline bool operator!=(detail::nullopt_t, optional<T> const &x)
{
  return bool(x);
}

/**
 * Creates an optional from the given value.
 *
 * @tparam T Type of optional value
 * @param v Value for optional
 * @return The created optional.
 */
template< typename T >
inline optional<T> make_optional(const T& v)
{
  return optional<T>(v);
}

}
#endif //MATADOR_OPTIONAL_HPP
