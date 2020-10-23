#ifndef MATADOR_OPTIONAL_HPP
#define MATADOR_OPTIONAL_HPP

#include <stdexcept>

namespace matador {

namespace detail {

struct nullopt_t
{
  struct init{};
  nullopt_t( init ) {}
};

const nullopt_t nullopt(( nullopt_t::init() ));

}

class bad_optional_access : public std::logic_error
{
public:
  explicit bad_optional_access()
    : logic_error( "bad optional access" ) {}
};

template < typename T >
class optional
{
public:
  typedef T value_type;

  optional() = default;
  optional(detail::nullopt_t) : has_value_(false) { };
  optional(const T &value)
    : has_value_(true), value_(value)
  {}

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

  const value_type* operator->() const
  {
    return assert( has_value() ), &value_;
  }

  value_type* operator->()
  {
    return assert( has_value() ), &value_;
  }

  const value_type& operator*() const
  {
    return assert( has_value() ), value_;
  }

  value_type& operator*()
  {
    return assert( has_value() ), value_;
  }

  explicit operator bool() const
  {
    return has_value();
  }

  const value_type& value() const
  {
    if (!has_value()) {
      throw bad_optional_access();
    }
    return value_;
  }

  value_type& value()
  {
    if (!has_value()) {
      throw bad_optional_access();
    }
    return value_;
  }

  template< class U >
  value_type value_or( U const & v ) const
  {
    return has_value() ? value() : static_cast<value_type>( v );
  }

  bool has_value() const
  {
    return has_value_;
  }

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

template< typename T >
inline optional<T> make_optional(const T& v)
{
  return optional<T>(v);
}

}
#endif //MATADOR_OPTIONAL_HPP
