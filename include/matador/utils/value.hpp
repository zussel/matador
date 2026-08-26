#ifndef QUERY_VALUE_HPP
#define QUERY_VALUE_HPP

#include "matador_export.h"

#include "matador/utils/basic_type_converter.hpp"
#include "matador/utils/default_type_traits.hpp"
#include "matador/utils/types.hpp"

#include <optional>

namespace matador::utils {

namespace detail {
template<typename Type>
size_t determine_size(const Type &/*val*/) { return 0; }
size_t determine_size(const std::string &val);
size_t determine_size(const char *val);
size_t determine_size(const blob_type_t &val);

}

class MATADOR_EXPORT value final {
public:
  value() = default;
  template<typename Type, std::enable_if_t<!std::is_enum_v<Type>>* = nullptr>
  explicit value(Type value, size_t size = 0)
  : value_(value)
  , size_(size)
  , type_(data_type_traits<Type>::type(size)) {}
  template<typename Type, std::enable_if_t<std::is_enum_v<Type>>* = nullptr>
  explicit value(Type value, size_t size = 0)
  : value_(static_cast<int>(value))
  , size_(size)
  , type_(data_type_traits<Type>::type(size)) {}
  explicit value(basic_type data_type, size_t size = 0);
  value(const value &x) = default;
  value& operator=(const value &x) = default;
  template<typename Type>
  value& operator=(Type val) {
    value_ = val;
    size_ = detail::determine_size(val);
    type_ = data_type_traits<Type>::type(size_);
    return *this;
  }
  value(value &&x) noexcept;
  value& operator=(value &&x) noexcept;

  bool operator==(const value& rhs) const;
  bool operator!=(const value& rhs) const;

  template<class Type>
  std::optional<Type> as() const {
    if (std::holds_alternative<Type>(value_)) {
      return std::get<Type>(value_);
    }
    const auto res = basic_type_converter<Type>::convert_value(value_);
    if (!res.is_ok()) {
      return std::nullopt;
    }
    return res.value();
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
  [[nodiscard]] basic_type type() const;
  void type(basic_type t);

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_floating_point() const;
  [[nodiscard]] bool is_bool() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_date() const;
  [[nodiscard]] bool is_time() const;
  [[nodiscard]] bool is_timestamp() const;
  [[nodiscard]] bool is_blob() const;
  [[nodiscard]] bool is_null() const;

  const database_type& raw_value() const;

private:
  database_type value_;
  size_t size_{};
  basic_type type_{basic_type::Null};

};

}
#endif //QUERY_VALUE_HPP
