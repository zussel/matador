#ifndef MATADOR_IDENTIFIER_HPP
#define MATADOR_IDENTIFIER_HPP

#include "matador_export.h"

#include "matador/utils/types.hpp"
#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <variant>

namespace matador::utils {
class value;
class identifier_serializer;

template<typename Type, class Enabled = void>
struct identifier_type_traits;

template<typename Type>
struct identifier_type_traits<Type, std::enable_if_t<std::is_integral_v<Type> && std::is_signed_v<Type> > > {
  static bool is_valid(Type value) { return value != 0; }
  static std::string to_string(const Type value) { return std::to_string(value); }
};

template<typename Type>
struct identifier_type_traits<Type, std::enable_if_t<std::is_integral_v<Type> && !std::is_signed_v<Type> > > {
  static bool is_valid(Type value) { return value > 0; }
  static std::string to_string(const Type value) { return std::to_string(value); }
};

template<typename Type>
struct identifier_type_traits<Type, std::enable_if_t<std::is_same_v<Type, std::string> > > {
  static bool is_valid(const Type &value) { return !value.empty(); }
  static std::string to_string(const Type &value) { return value; }
};

template<>
struct identifier_type_traits<null_type_t, void> {
  static bool is_valid() { return false; }
  static std::string to_string() { return "null"; }
};

namespace detail {
template<typename Type>
size_t hash(const Type &value) {
  return std::hash<Type>()(value);
}

size_t hash(const std::string &value);
}

template <class T>
struct is_identifier_supported : std::false_type {};

template <> struct is_identifier_supported<int8_t> : std::true_type {};
template <> struct is_identifier_supported<int16_t> : std::true_type {};
template <> struct is_identifier_supported<int32_t> : std::true_type {};
template <> struct is_identifier_supported<int64_t> : std::true_type {};
template <> struct is_identifier_supported<uint8_t> : std::true_type {};
template <> struct is_identifier_supported<uint16_t> : std::true_type {};
template <> struct is_identifier_supported<uint32_t> : std::true_type {};
template <> struct is_identifier_supported<uint64_t> : std::true_type {};
template <> struct is_identifier_supported<std::string> : std::true_type {};

template <class T>
inline constexpr bool is_identifier_supported_v = is_identifier_supported<std::decay_t<T>>::value;

class MATADOR_EXPORT identifier {
public:
  using value_type = std::variant<
    std::monostate,
    int8_t, int16_t, int32_t, int64_t,
    uint8_t, uint16_t, uint32_t, uint64_t,
    std::string
  >;

  identifier();
  template<typename Type, std::enable_if_t<is_identifier_supported_v<Type>, int> = 0>
  explicit identifier(Type &&id)
  : value_(std::forward<Type>(id)) {
  }

  identifier(const identifier &x) = default;
  identifier &operator=(const identifier &x);
  identifier(identifier &&x) noexcept = default;
  identifier &operator=(identifier &&x) noexcept = default;

  explicit identifier(std::nullptr_t);
  explicit identifier(const char *value);

  identifier& operator=(std::nullptr_t);

  template<typename Type, std::enable_if_t<std::is_integral_v<std::decay_t<Type>> && !std::is_same_v<std::decay_t<Type>, bool>, int> = 0>
  identifier& operator=(Type value) {
    assign_integral(value);
    return *this;
  }

  identifier& operator=(const std::string &value);
  identifier& operator=(const char *value);
  template<typename Type, std::enable_if_t<is_identifier_supported_v<Type>  && std::is_same_v<std::decay_t<Type>, bool>, int> = 0>
  identifier& operator=(Type &&value) {
    value_ = std::forward<Type>(value);
    return *this;
  }

  ~identifier() = default;

  result<void, error> assign(const value &val);

  static result<identifier, error> from_value(const value &val);
  [[nodiscard]] database_type to_database_type() const;

  bool operator==(const identifier &x) const;
  bool operator!=(const identifier &x) const;
  bool operator<(const identifier &x) const;
  bool operator<=(const identifier &x) const;
  bool operator>(const identifier &x) const;
  bool operator>=(const identifier &x) const;

  [[nodiscard]] std::string str() const;
  [[nodiscard]] const std::type_index &type_index() const;
  [[nodiscard]] basic_type type() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_null() const;

  [[nodiscard]] bool is_valid() const;
  void clear();

  void serialize(identifier_serializer &s) const;

  [[nodiscard]] size_t hash() const;

  template <typename Type, std::enable_if_t<is_identifier_supported_v<Type>, int> = 0>
  result<Type, error> as() const;

  template <typename Type, std::enable_if_t<std::is_integral_v<Type> && !std::is_same_v<Type, bool>, int> = 0>
  result<Type, error> convert() const;

  friend std::ostream &operator<<(std::ostream &out, const identifier &id);

private:
  template<typename Type>
  void assign_integral(Type value) {
    if constexpr (std::is_signed_v<Type>) {
      if (sizeof(Type) <= sizeof(int8_t)) {
        value_ = static_cast<int8_t>(value);
      } else if (sizeof(Type) <= sizeof(int16_t)) {
        value_ = static_cast<int16_t>(value);
      } else if (sizeof(Type) <= sizeof(int32_t)) {
        value_ = static_cast<int32_t>(value);
      } else {
        value_ = static_cast<int64_t>(value);
      }
    } else {
      if (sizeof(Type) <= sizeof(uint8_t)) {
        value_ = static_cast<uint8_t>(value);
      } else if (sizeof(Type) <= sizeof(uint16_t)) {
        value_ = static_cast<uint16_t>(value);
      } else if (sizeof(Type) <= sizeof(uint32_t)) {
        value_ = static_cast<uint32_t>(value);
      } else {
        value_ = static_cast<uint64_t>(value);
      }
    }
  }

  static size_t type_rank(const value_type &value);
  static std::type_index type_index_from_value(const value_type &value);
  static basic_type type_from_value(const value_type &value);

private:
  value_type value_{std::monostate{}};
};

template <typename Target, typename Source>
bool in_range(Source value) {
  if constexpr (std::is_signed_v<Source> == std::is_signed_v<Target>) {
    return value >= static_cast<Target>((std::numeric_limits<Source>::min)()) &&
           value <= static_cast<Target>((std::numeric_limits<Source>::max)());
  } else if constexpr (std::is_signed_v<Source> && !std::is_signed_v<Target>) {
    if (value < 0) {
      return false;
    }
    using UnsignedSource = std::make_unsigned_t<Source>;
    return static_cast<UnsignedSource>(value) <= (std::numeric_limits<Target>::max)();
  } else {
    return value <= static_cast<std::make_unsigned_t<Target>>((std::numeric_limits<Target>::max)());
  }
}

template <typename Type, std::enable_if_t<is_identifier_supported_v<Type>, int>>
result<Type, error> identifier::as() const {
  return std::visit([](const auto &v) -> result<Type, error> {
    using StoredType = std::decay_t<decltype(v)>;

    if constexpr (std::is_same_v<StoredType, std::monostate>) {
      return failure(error{});
    } else if constexpr (std::is_same_v<StoredType, Type>) {
      return ok<Type>(v);
    } else {
      return failure(error{});
    }
  }, value_);
}

template <typename Type, std::enable_if_t<std::is_integral_v<Type> && !std::is_same_v<Type, bool>, int>>
result<Type, error> identifier::convert() const {
  return std::visit([](const auto &v) -> result<Type, error> {
    using Stored = std::decay_t<decltype(v)>;

    if constexpr (std::is_same_v<Stored, std::monostate>) {
      return failure(error{});
    } else if constexpr (std::is_integral_v<Stored>) {
      if (!in_range<Type>(v)) {
        return failure(error{});
      }
      return ok<Type>(static_cast<Type>(v));
    } else {
      return failure(error{});
    }
  }, value_);
}

static identifier null_identifier{};

/// @cond MATADOR_DEV
struct id_pk_hash {
  size_t operator()(const identifier &id) const;
};

/// @endcond
}

template<>
struct std::hash<matador::utils::identifier> {
  size_t operator()(const matador::utils::identifier &id) const noexcept {
    return id.hash();
  }
}; // namespace std

#endif //MATADOR_IDENTIFIER_HPP