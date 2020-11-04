#ifndef MATADOR_OPTIONAL_HPP
#define MATADOR_OPTIONAL_HPP

#include <stdexcept>

namespace matador {

namespace detail {

struct nullopt_t
{
  struct init{};
  explicit nullopt_t( init ) {}
};

const nullopt_t nullopt(( nullopt_t::init() ));

}

class bad_optional_access : public std::logic_error
{
public:
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

  optional& operator=(detail::nullopt_t)
  {
    reset();
    return *this;
  }

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

template< typename T, typename U >
inline bool operator==( optional<T> const & x, optional<U> const & y )
{
  return bool(x) == bool(y) && (!bool(x) || *x == *y);
}

template< typename T, typename U >
inline bool operator!=( optional<T> const & x, optional<U> const & y )
{
  return !(x == y);
}

template< typename T, typename U >
inline bool operator<( optional<T> const & x, optional<U> const & y )
{
  return y && ((!x) || *x < *y);
}

template< typename T, typename U >
inline bool operator>( optional<T> const & x, optional<U> const & y )
{
  return (y < x);
}

template< typename T, typename U >
inline bool operator<=( optional<T> const & x, optional<U> const & y )
{
  return !(y < x);
}

template< typename T, typename U >
inline bool operator>=( optional<T> const & x, optional<U> const & y )
{
  return !(x < y);
}

template< typename T>
inline bool operator==(optional<T> const &x, detail::nullopt_t)
{
  return (!x);
}

template< typename T>
inline bool operator==(detail::nullopt_t, optional<T> const &x)
{
  return (!x);
}

template< typename T>
inline bool operator!=(optional<T> const &x, detail::nullopt_t)
{
  return bool(x);
}

template< typename T>
inline bool operator!=(detail::nullopt_t, optional<T> const &x)
{
  return bool(x);
}

template< typename T >
inline optional<T> make_optional(const T& v)
{
  return optional<T>(v);
}

}
#endif //MATADOR_OPTIONAL_HPP
