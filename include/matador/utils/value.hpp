#ifndef QUERY_VALUE_HPP
#define QUERY_VALUE_HPP

#include "matador/utils/any_type_converter.hpp"
#include "matador/utils/default_type_traits.hpp"
#include "matador/utils/types.hpp"

#include <optional>

namespace matador::utils {

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
class value
{
public:
  value() = default;
  template<typename Type>
  explicit value(Type value, size_t size = 0)
  : value_(value)
  , size_(size)
  , type_(data_type_traits<Type>::type(size)) {}
  explicit value(data_type data_type, size_t size = 0);
  value(const value &x) = default;
  value& operator=(const value &x) = default;
  template<typename Type>
  value& operator=(Type val)
  {
    value_ = val;
    size_ = detail::determine_size(val);
    type_ = data_type_traits<Type>::type(size_);
    return *this;
  }
  value(value &&x) noexcept;
  value& operator=(value &&x) noexcept;

  template<class Type>
  std::optional<Type> as() const
  {
    if (std::holds_alternative<Type>(value_)) {
      return std::get<Type>(value_);
    } else {
      any_type_converter<Type> visitor;
      std::visit(visitor, const_cast<utils::any_type &>(value_));
      return visitor.result;
    }
  }

  template<class Type>
  std::optional<std::reference_wrapper<Type>> ref() {
    if (std::holds_alternative<Type>(value_)) {
      return std::get<Type>(value_);
    }

    return std::nullopt;
  }

  [[nodiscard]] std::string str() const;

  [[nodiscard]] size_t size() const;
  [[nodiscard]] data_type type() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_floating_point() const;
  [[nodiscard]] bool is_bool() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_date() const;
  [[nodiscard]] bool is_time() const;
  [[nodiscard]] bool is_blob() const;
  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_unknown() const;

private:
  utils::any_type value_;
  size_t size_{};
  data_type type_{data_type::type_null};

};

}
#endif //QUERY_VALUE_HPP
