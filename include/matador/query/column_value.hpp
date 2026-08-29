#ifndef QUERY_VALUE_HPP
#define QUERY_VALUE_HPP

#include "matador_export.h"

#include "matador/utils/convert.hpp"
#include "matador/utils/default_type_traits.hpp"
#include "matador/utils/result.hpp"
#include "matador/utils/types.hpp"

#include <optional>

namespace matador::query {
namespace detail {
template<typename Type>
size_t determine_size(const Type &/*val*/) { return 0; }
size_t determine_size(const std::string &val);
size_t determine_size(const char *val);
size_t determine_size(const utils::blob_type_t &val);

template < typename Type >
class basic_type_converter {
public:
  static result<Type, utils::conversion_error> convert_value(const utils::database_type& from) {
    basic_type_converter converter;
    std::visit(converter, const_cast<utils::database_type&>(from));

    return converter.result_;
  }

  void operator()(int8_t &x) { this->convert(x); }
  void operator()(int16_t &x) { this->convert(x); }
  void operator()(int32_t &x) { this->convert(x); }
  void operator()(int64_t &x) { this->convert(x); }
  void operator()(uint8_t &x) { this->convert(x); }
  void operator()(uint16_t &x) { this->convert(x); }
  void operator()(uint32_t &x) { this->convert(x); }
  void operator()(uint64_t &x) { this->convert(x); }
  void operator()(bool &x) { this->convert(x); }
  void operator()(float &x) { this->convert(x); }
  void operator()(double &x) { this->convert(x); }
  void operator()(const char *x) { this->convert(x); }
  void operator()(std::string &x) { this->convert(x); }
  void operator()(utils::date_type_t &x) { this->convert(x); }
  void operator()(utils::time_type_t &x) { this->convert(x); }
  void operator()(utils::timestamp_type_t &x) { this->convert(x); }
  void operator()(utils::blob_type_t &x) { this->convert(x); }

private:
  result<Type, utils::conversion_error> result_{};

  template< typename FromType >
  void convert(FromType &from) {
    result_ = utils::to<Type>(from);
  }
};
}

class MATADOR_EXPORT column_value final {
public:
  column_value() = default;
  template<typename Type, std::enable_if_t<!std::is_enum_v<Type>>* = nullptr>
  explicit column_value(Type value, size_t size = 0)
  : value_(value)
  , size_(size)
  , type_(utils::data_type_traits<Type>::type(size)) {}
  template<typename Type, std::enable_if_t<std::is_enum_v<Type>>* = nullptr>
  explicit column_value(Type value, size_t size = 0)
  : value_(static_cast<int>(value))
  , size_(size)
  , type_(utils::data_type_traits<Type>::type(size)) {}
  explicit column_value(utils::basic_type data_type, size_t size = 0);
  column_value(const column_value &x) = default;
  column_value& operator=(const column_value &x) = default;
  template<typename Type>
  column_value& operator=(Type val) {
    value_ = val;
    size_ = detail::determine_size(val);
    type_ = utils::data_type_traits<Type>::type(size_);
    return *this;
  }
  column_value(column_value &&x) noexcept;
  column_value& operator=(column_value &&x) noexcept;

  bool operator==(const column_value& rhs) const;
  bool operator!=(const column_value& rhs) const;

  template<class Type>
  std::optional<Type> as() const {
    if (std::holds_alternative<Type>(value_)) {
      return std::get<Type>(value_);
    }
    const auto res = detail::basic_type_converter<Type>::convert_value(value_);
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
  [[nodiscard]] utils::basic_type type() const;
  void type(utils::basic_type t);

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

  [[nodiscard]] const utils::database_type& raw_value() const;

private:
  utils::database_type value_;
  size_t size_{};
  utils::basic_type type_{utils::basic_type::Null};

};

}
#endif //QUERY_VALUE_HPP
