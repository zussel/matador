#include "matador/utils/identifier.hpp"

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/identifier_serializer.hpp"
#include "matador/utils/value.hpp"
#include "matador/utils/errors.hpp"

#include <utility>

namespace matador::utils {

namespace {
inline const std::type_index& null_type_index() {
  static const std::type_index value{typeid(null_type_t)};
  return value;
}

inline const std::type_index& i8_type_index() {
  static const std::type_index value{typeid(int8_t)};
  return value;
}

inline const std::type_index& i16_type_index() {
  static const std::type_index value{typeid(int16_t)};
  return value;
}

inline const std::type_index& i32_type_index() {
  static const std::type_index value{typeid(int32_t)};
  return value;
}

inline const std::type_index& i64_type_index() {
  static const std::type_index value{typeid(int64_t)};
  return value;
}

inline const std::type_index& u8_type_index() {
  static const std::type_index value{typeid(uint8_t)};
  return value;
}

inline const std::type_index& u16_type_index() {
  static const std::type_index value{typeid(uint16_t)};
  return value;
}

inline const std::type_index& u32_type_index() {
  static const std::type_index value{typeid(uint32_t)};
  return value;
}

inline const std::type_index& u64_type_index() {
  static const std::type_index value{typeid(uint64_t)};
  return value;
}

inline const std::type_index& string_type_index() {
  static const std::type_index value{typeid(std::string)};
  return value;
}
} // namespace

size_t detail::hash(const std::string &value) {
  return std::hash<std::string_view>()(std::string_view(value));
}

identifier::identifier()
: value_(std::monostate{}) {
}

size_t identifier::type_rank(const value_type &value) {
  return value.index();
}

std::type_index identifier::type_index_from_value(const value_type &value) {
  return std::visit([](const auto &v) -> std::type_index {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      return null_type_index();
    } else {
      return std::type_index(typeid(T));
    }
  }, value);
}

const std::type_index &identifier::type_index() const {
  switch (value_.index()) {
    case 0: return null_type_index();
    case 1: return i8_type_index();
    case 2: return i16_type_index();
    case 3: return i32_type_index();
    case 4: return i64_type_index();
    case 5: return u8_type_index();
    case 6: return u16_type_index();
    case 7: return u32_type_index();
    case 8: return u64_type_index();
    case 9: return string_type_index();
    default: return null_type_index();
  }
}

basic_type identifier::type_from_value(const value_type &value) {
  return std::visit([](const auto &v) -> basic_type {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      return basic_type::Null;
    } else {
      return data_type_traits<T>::type(1);
    }
  }, value);
}

identifier::identifier(std::nullptr_t)
: value_(std::monostate{}) {
}

identifier::identifier(const char *value)
: value_(value ? std::string(value) : std::string{}) {
}

identifier & identifier::operator=(const char *value) {
  value_ = value ? std::string(value) : std::string{};
  return *this;
}

result<identifier, error> identifier::from_value(const value &val) {
  identifier id;
  if (const auto result = id.assign(val); result.is_error()) {
    return failure(result.err());
  }

  return ok(id);
}

database_type identifier::to_database_type() const {
  return std::visit([](const auto &v) -> database_type {
          using T = std::decay_t<decltype(v)>;

          if constexpr (std::is_same_v<T, std::monostate>) {
            return nullptr;
          } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
          } else {
            return static_cast<T>(v);
          }
        }, value_);
}

result<void, error> identifier::assign(const value &val) {
  switch (val.type()) {
    case basic_type::Null:
      value_ = std::monostate{};
      return ok<void>{};

    case basic_type::Int8:
      if (auto v = val.as<int8_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::Int16:
      if (auto v = val.as<int16_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::Int32:
      if (auto v = val.as<int32_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::Int64:
      if (auto v = val.as<int64_t>()) { value_ = *v; return ok<void>{}; }
      break;

    case basic_type::UInt8:
      if (auto v = val.as<uint8_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::UInt16:
      if (auto v = val.as<uint16_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::UInt32:
      if (auto v = val.as<uint32_t>()) { value_ = *v; return ok<void>{}; }
      break;
    case basic_type::UInt64:
      if (auto v = val.as<uint64_t>()) { value_ = *v; return ok<void>{}; }
      break;

    case basic_type::Text:
    case basic_type::Varchar:
      if (auto v = val.as<std::string>()) { value_ = *v; return ok<void>{}; }
      break;

    default:
      break;
  }

  return failure(error{utils_error::IdentifierTypeMismatch});
}

bool identifier::operator==(const identifier &x) const {
  return value_ == x.value_;
}

bool identifier::operator!=(const identifier &x) const {
  return !operator==(x);
}

identifier &identifier::operator=(std::nullptr_t) {
  value_ = std::monostate{};
  return *this;
}

identifier &identifier::operator=(const std::string &value) {
  value_ = value;
  return *this;
}

identifier &identifier::operator=(const identifier &other) {
  if (this != &other) {
    value_ = other.value_;
  }
  return *this;
}

bool identifier::operator<(const identifier &x) const {
  if (value_.index() != x.value_.index()) {
    return value_.index() < x.value_.index();
  }

  return std::visit([](const auto &lhs, const auto &rhs) -> bool {
    using L = std::decay_t<decltype(lhs)>;
    using R = std::decay_t<decltype(rhs)>;
    if constexpr (std::is_same_v<L, R>) {
      return lhs < rhs;
    } else {
      return false;
    }
  }, value_, x.value_);
}

bool identifier::operator<=(const identifier &x) const {
  return operator==(x) || operator<(x);
}

bool identifier::operator>(const identifier &x) const {
  return !operator<=(x);
}

bool identifier::operator>=(const identifier &x) const {
  return !operator<(x);
}

std::string identifier::str() const {
  return std::visit([](const auto &v) -> std::string {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      return identifier_type_traits<null_type_t>::to_string();
    } else if constexpr (std::is_same_v<T, std::string>) {
      return identifier_type_traits<std::string>::to_string(v);
    } else {
      return identifier_type_traits<T>::to_string(v);
    }
  }, value_);
}

basic_type identifier::type() const {
  return type_from_value(value_);
}

bool identifier::is_integer() const {
  return std::holds_alternative<int8_t>(value_)
      || std::holds_alternative<int16_t>(value_)
      || std::holds_alternative<int32_t>(value_)
      || std::holds_alternative<int64_t>(value_)
      || std::holds_alternative<uint8_t>(value_)
      || std::holds_alternative<uint16_t>(value_)
      || std::holds_alternative<uint32_t>(value_)
      || std::holds_alternative<uint64_t>(value_);
}

bool identifier::is_varchar() const {
  return std::holds_alternative<std::string>(value_);
}

bool identifier::is_null() const {
  return std::holds_alternative<std::monostate>(value_);
}

bool identifier::is_valid() const {
  return std::visit([](const auto &v) -> bool {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      return identifier_type_traits<null_type_t>::is_valid();
    } else {
      return identifier_type_traits<T>::is_valid(v);
    }
  }, value_);
}

void identifier::clear() {
  value_ = std::monostate{};
}

void identifier::serialize(identifier_serializer &s) const {
  std::visit([&s](const auto &v) {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      null_type_t n{};
      s.serialize(n, {});
    } else {
      auto tmp = v;
      s.serialize(tmp, {});
    }
  }, value_);
}

size_t identifier::hash() const {
  return std::visit([](const auto &v) -> size_t {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::monostate>) {
      return std::hash<int>{}(0);
    } else {
      return detail::hash(v);
    }
  }, value_);
}

std::ostream &operator<<(std::ostream &out, const identifier &id) {
  out << id.str();
  return out;
}

size_t id_pk_hash::operator()(const identifier &id) const {
  return id.hash();
}

} // namespace matador::utils