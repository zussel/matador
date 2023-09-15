#ifndef VALUE_HPP
#define VALUE_HPP

#include "matador/sql/token.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/any.hpp"

#include <string>
#include <typeinfo>
#include <cstring>
#include <type_traits>

namespace matador {

/// @cond MATADOR_DEV

struct value : public detail::token
{
  value();

  template<typename T, typename U = std::decay<T>>
  explicit value(T &&val)
    : token(detail::token::VALUE)
    , value_(val)
  {}

  template<typename T, typename U = std::decay<T>>
  value(T &&val, std::size_t s)
    : token(detail::token::VALUE)
    , value_(val)
    , size_(s)
  {}

  template < class T > T get() {
    return value_._<T>();
  }

  void accept(token_visitor &visitor) override;

  any value_;
  std::size_t size_ = 0;
};

template < class T >
value* make_value(const T &val)
{
  return new value(val);
}

/// @endcond

}

#endif /* VALUE_HPP */
