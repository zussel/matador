#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <variant>
#include <optional>
#include <functional>
#include <type_traits>

namespace matador::utils {

template < typename ValueType, typename ErrorType >
class [[nodiscard]] result;

template <typename ValueType>
struct is_result : std::false_type {};

template < typename ValueType, typename ErrorType >
struct is_result<result<ValueType, ErrorType>> : std::true_type {};

template < typename ValueType >
class ok
{
public:
  using value_type = ValueType;
  explicit constexpr ok(const ValueType &value) : value_(value) {}
  explicit constexpr ok(ValueType &&value) : value_(std::move(value)) {}

  constexpr ValueType&& release() { return std::move(value_); }
  const ValueType& value() const { return value_; }
  ValueType& value() { return value_; }

private:
  ValueType value_;
};

template <>
class ok<void>
{
public:
  using value_type = void;
  explicit constexpr ok() = default;
};

template < typename ErrorType >
class error
{
public:
  using value_type = ErrorType;

  explicit constexpr error(const ErrorType &error) : error_(error) {}
  explicit constexpr error(ErrorType &&error) : error_(std::move(error)) {}

  constexpr ErrorType&& release() { return std::move(error_); }
  const ErrorType& value() const { return error_; }
  ErrorType value() { return error_; }

private:
  ErrorType error_;
};

template < typename ValueType, typename ErrorType >
class result
{
public:
  using value_type = ValueType;
  using error_type = ErrorType;

  result() : result_(ValueType{}) {}
  result(ok<value_type> value) : result_(std::move(value.release())) {} // NOLINT(*-explicit-constructor)
  result(error<error_type> error) : result_(std::move(error.release())) {} // NOLINT(*-explicit-constructor)
  result(const result<ValueType, ErrorType> &x) = default;
  result& operator=(const result<ValueType, ErrorType> &x) = default;
  result(result<ValueType, ErrorType> &&x) = default;
  result& operator=(result<ValueType, ErrorType> &&x) = default;

  operator bool() const { return is_ok(); } // NOLINT(*-explicit-constructor)

  [[nodiscard]] bool is_ok() const { return std::holds_alternative<value_type>(result_); }
  [[nodiscard]] bool is_error() const { return std::holds_alternative<error_type>(result_); }

  ValueType&& release() { return std::move(std::get<value_type>(result_)); }
  ErrorType&& release_error() { return std::move(std::get<error_type>(result_)); }

  const ValueType& value() const { return std::get<value_type>(result_); }
  ValueType& value() { return std::get<value_type>(result_); }
  const ErrorType& err() const { return std::get<error_type>(result_); }
  ErrorType err() { return std::get<error_type>(result_); }

  constexpr const ValueType* operator->() const { return &value(); }
  constexpr ValueType* operator->() { return &std::get<value_type>(result_); }

  constexpr const ValueType& operator*() const& noexcept { return value(); }
  constexpr ValueType& operator*() & noexcept { return value(); }

  template<typename Func, typename SecondValueType = std::invoke_result_t<Func, ValueType >>
  result<SecondValueType, ErrorType> transform(Func &&f) {
    if (is_ok()) {
      return result<SecondValueType, ErrorType>(ok(f(release())));
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

  template<typename Func,
           typename SecondValueType = typename std::invoke_result_t<Func, ValueType>::value_type>
  result<SecondValueType, ErrorType> and_then(Func &&f) {
    if (is_ok()) {
      return f(release());
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

//  result<void, ErrorType> and_then(std::function<result<void, ErrorType>()> &&f) {
//    if (is_ok()) {
//      return f();
//    }
//
//    return result<void, ErrorType>(error(release_error()));
//  }

  template<typename Func,
           typename SecondErrorType = typename std::invoke_result_t<Func, ErrorType >::value_type>
  result<ValueType, SecondErrorType> or_else(Func &&f) {
    if (is_error()) {
      return f(err());
    }

    return result<ValueType, SecondErrorType>(ok(release()));
  }

private:
  std::variant<value_type, error_type> result_;
};

template < typename ErrorType >
class result<void, ErrorType>
{
public:
  using value_type = ok<void>;
  using error_type = error<ErrorType>;

  result() = default;
  result(value_type /*value*/) {};
  result(error_type error) : result_(std::move(error)) {} // NOLINT(*-explicit-constructor)
  result(const result<void, ErrorType> &x) = default;
  result& operator=(const result<void, ErrorType> &x) = default;
  result(result<void, ErrorType> &&x) = default;
  result& operator=(result<void, ErrorType> &&x) = default;

  operator bool() const { return is_ok(); } // NOLINT(*-explicit-constructor)

  [[nodiscard]] bool is_ok() const { return !result_.has_value(); }
  [[nodiscard]] bool is_error() const { return result_.has_value(); }

  ErrorType&& release_error() { return result_->release(); }

  const ErrorType& err() const { return std::get<error_type>(result_).value(); }
  ErrorType err() { return std::get<error_type>(result_).value(); }

  template<typename Func, typename SecondValueType = typename std::invoke_result_t<Func>>
  result<SecondValueType, ErrorType> transform(Func &&f) {
    if (is_ok()) {
      return result<SecondValueType, ErrorType>(ok(f()));
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

  template<typename Func, typename SecondValueType = typename std::invoke_result_t<Func>::value_type>
  result<SecondValueType, ErrorType> and_then(Func &&f) {
    if (is_ok()) {
      return f();
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

  template<typename Func, typename SecondErrorType = typename std::invoke_result_t<Func, ErrorType >::error_type>
  result<void, SecondErrorType> or_else(Func &&f) {
    if (is_error()) {
      return f(err());
    }

    return result<void, SecondErrorType>(ok<void>());
  }

private:
  std::optional<error_type> result_;
};

}

#endif //QUERY_RESULT_HPP
