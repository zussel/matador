#ifndef MATADOR_DATA_TYPE_HPP
#define MATADOR_DATA_TYPE_HPP

#include "matador/utils/types.hpp"

namespace matador::utils {
class data_type {
public:
  constexpr explicit data_type(basic_type type, std::size_t size = 0) noexcept
  : type_(type)
  , size_(size) {
  }

  [[nodiscard]] constexpr basic_type type() const noexcept {
    return type_;
  }

  /**
   * Returns the declared database size/length for variable-size types.
   * Returns 0 if the type has no declared size.
   *
   * @return the declared database size/length
   */
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

private:
  basic_type type_{basic_type::Unknown};
  std::size_t size_{0};
};

constexpr bool operator==(const data_type& lhs, const data_type& rhs) noexcept {
  return lhs.type() == rhs.type() && lhs.size() == rhs.size();
}

constexpr bool operator!=(const data_type& lhs, const data_type& rhs) noexcept {
  return !(lhs == rhs);
}

namespace detail {
template<typename Type>
struct is_data_type_descriptor : std::false_type {
};

template<>
struct is_data_type_descriptor<data_type> : std::true_type {
};

template<basic_type Type>
class fixed_data_type;

template<basic_type Type>
class sized_data_type;

template<basic_type Type>
struct is_data_type_descriptor<fixed_data_type<Type>> : std::true_type {
};

template<basic_type Type>
struct is_data_type_descriptor<sized_data_type<Type>> : std::true_type {
};

template<typename Type>
inline constexpr bool is_data_type_descriptor_v =
  is_data_type_descriptor<std::decay_t<Type>>::value;

template<typename Left, typename Right>
using enable_if_data_type_descriptors_t = std::enable_if_t<
  is_data_type_descriptor_v<Left> &&
  is_data_type_descriptor_v<Right>,
  bool
>;

template<basic_type Type>
class fixed_data_type {
public:
  constexpr fixed_data_type() noexcept = default;

  [[nodiscard]] constexpr basic_type type() const noexcept {
    return Type;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return 0;
  }

  [[nodiscard]] constexpr operator data_type() const noexcept {
    return data_type{type(), size()};
  }
};

template<basic_type Type>
class sized_data_type {
public:
  constexpr explicit sized_data_type(std::size_t size) noexcept
  : size_(size) {
  }

  [[nodiscard]] constexpr basic_type type() const noexcept {
    return Type;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr operator data_type() const noexcept {
    return data_type{type(), size()};
  }

private:
  std::size_t size_{0};
};

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator==(const Left &lhs, const Right &rhs) noexcept {
  return lhs.type() == rhs.type() && lhs.size() == rhs.size();
}

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator!=(const Left &lhs, const Right &rhs) noexcept {
  return !(lhs == rhs);
}

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator<(const Left &lhs, const Right &rhs) noexcept {
  if (lhs.type() != rhs.type()) {
    return lhs.type() < rhs.type();
  }

  return lhs.size() < rhs.size();
}

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator<=(const Left &lhs, const Right &rhs) noexcept {
  return !(rhs < lhs);
}

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator>(const Left &lhs, const Right &rhs) noexcept {
  return rhs < lhs;
}

template<typename Left, typename Right>
[[nodiscard]] constexpr enable_if_data_type_descriptors_t<Left, Right>
operator>=(const Left &lhs, const Right &rhs) noexcept {
  return !(lhs < rhs);
}
}
}

namespace matador {
using TinyInt = utils::detail::fixed_data_type<utils::basic_type::Int8>;
using SmallInt = utils::detail::fixed_data_type<utils::basic_type::Int16>;
using Integer = utils::detail::fixed_data_type<utils::basic_type::Int32>;
using BigInt = utils::detail::fixed_data_type<utils::basic_type::Int64>;

using TinyIntUnsigned = utils::detail::fixed_data_type<utils::basic_type::UInt8>;
using SmallIntUnsigned = utils::detail::fixed_data_type<utils::basic_type::UInt16>;
using IntegerUnsigned = utils::detail::fixed_data_type<utils::basic_type::UInt32>;
using BigIntUnsigned = utils::detail::fixed_data_type<utils::basic_type::UInt64>;

using Real = utils::detail::fixed_data_type<utils::basic_type::Float>;
using Double = utils::detail::fixed_data_type<utils::basic_type::Double>;

using Text = utils::detail::fixed_data_type<utils::basic_type::Text>;
using Boolean = utils::detail::fixed_data_type<utils::basic_type::Boolean>;
using Varchar = utils::detail::sized_data_type<utils::basic_type::Varchar>;
using Blob = utils::detail::sized_data_type<utils::basic_type::Blob>;

using Time = utils::detail::fixed_data_type<utils::basic_type::Time>;
using Date = utils::detail::fixed_data_type<utils::basic_type::Date>;
using Timestamp = utils::detail::fixed_data_type<utils::basic_type::DateTime>;

using Null = utils::detail::fixed_data_type<utils::basic_type::Null>;
}
#endif // MATADOR_DATA_TYPE_HPP
