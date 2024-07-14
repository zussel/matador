#ifndef VALUE_HPP
#define VALUE_HPP

#include "matador/sql/token.hpp"
#include "matador/sql/column_value.hpp"
#include "matador/sql/column_value_visitor.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/serializer.hpp"

#include <string>
#include <optional>
#include <typeinfo>
#include <cstring>
#include <type_traits>

namespace matador {

/// @cond MATADOR_DEV

namespace detail {
template<typename Type>
size_t determine_size(const Type &/*val*/)
{
  return 0;
}
size_t determine_size(const std::string &val);
size_t determine_size(const char *val);
size_t determine_size(const utils::blob &val);

}

class value : public detail::token
{
public:
  value();

  template<typename Type>
  explicit value(Type val, size_t size = 0)
    : token(detail::token::VALUE)
    , value_(val)
    , size_(size)
    , type_(data_type_traits<Type>::builtin_type(size))
  {}
  explicit value(matador::data_type data_type, size_t size = 0);
  explicit value(sql::column_type value, size_t size = 0);
  value(const value &x) = default;
  value& operator=(const value &x) = default;
  template<typename Type>
  value& operator=(Type val)
  {
    value_ = val;
    size_ = detail::determine_size(val);
    type_ = data_type_traits<Type>::builtin_type(size_);
    return *this;
  }
  value(value &&x) noexcept;
  value& operator=(value &&x) noexcept;

  void accept(token_visitor &visitor) override;

  template<class Type>
  std::optional<Type> as() const
  {
    if (std::holds_alternative<Type>(value_)) {
      return std::get<Type>(value_);
    } else {
      sql::column_value_visitor<Type> visitor;
      std::visit(visitor, const_cast<sql::column_type&>(value_));
      return visitor.result;
    }
  }
  [[nodiscard]] std::string str() const;

  [[nodiscard]] size_t size() const;
  [[nodiscard]] matador::data_type data_type() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_floating_point() const;
  [[nodiscard]] bool is_bool() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_blob() const;
  [[nodiscard]] bool is_date() const;
  [[nodiscard]] bool is_time() const;
  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_unknown() const;

  const sql::column_type& operator()() const;

private:
  sql::column_type value_;
  std::size_t size_ = 0;
  matador::data_type type_{data_type::type_null};
};

template < class T >
value* make_value(const T &val)
{
  return new value(val);
}

/// @endcond

}

#endif /* VALUE_HPP */
