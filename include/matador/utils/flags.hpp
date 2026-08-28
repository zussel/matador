#ifndef MATADOR_FLAGS_HPP
#define MATADOR_FLAGS_HPP

#include <cstdint>
#include <type_traits>

namespace matador::utils {
template <typename Enum>
class flags {
  static_assert(std::is_enum_v<Enum>, "flags<T> requires an enum type");

public:
  using enum_type = Enum;
  using underlying_type = std::underlying_type_t<Enum>;

  constexpr flags() = default;

  constexpr flags(Enum value)
  : value_(to_underlying(value)) {}

  constexpr explicit flags(underlying_type value)
  : value_(value) {}

  [[nodiscard]] constexpr bool empty() const {
    return value_ == 0;
  }

  [[nodiscard]] constexpr bool has(Enum value) const {
    return (value_ & to_underlying(value)) != 0;
  }

  constexpr flags& set(Enum value) {
    value_ |= to_underlying(value);
    return *this;
  }

  constexpr flags& unset(Enum value) {
    value_ &= ~to_underlying(value);
    return *this;
  }

  constexpr flags& clear() {
    value_ = 0;
    return *this;
  }

  [[nodiscard]] constexpr underlying_type value() const {
    return value_;
  }

  explicit constexpr operator bool() const {
    return !empty();
  }

  constexpr flags& operator|=(flags other) {
    value_ |= other.value_;
    return *this;
  }

  constexpr flags& operator&=(flags other) {
    value_ &= other.value_;
    return *this;
  }

  friend constexpr flags operator|(flags lhs, flags rhs) {
    lhs |= rhs;
    return lhs;
  }

  friend constexpr flags operator&(flags lhs, flags rhs) {
    lhs &= rhs;
    return lhs;
  }

  friend constexpr flags operator|(Enum lhs, Enum rhs) {
    return flags(lhs) | flags(rhs);
  }

  friend constexpr flags operator&(Enum lhs, Enum rhs) {
    return flags(lhs) & flags(rhs);
  }

private:
  [[nodiscard]] static constexpr underlying_type to_underlying(Enum value) {
    return static_cast<underlying_type>(value);
  }

  underlying_type value_{};
};
} // namespace matador::utils

#endif //MATADOR_FLAGS_HPP
