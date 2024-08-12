#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <variant>

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
  using value_type = ok<ValueType>;
  using error_type = error<ErrorType>;

  result() : result_(ValueType{}) {}
  result(value_type value) : result_(std::move(value)) {} // NOLINT(*-explicit-constructor)
  result(error_type error) : result_(std::move(error)) {} // NOLINT(*-explicit-constructor)
  result(const result<ValueType, ErrorType> &x) = default;
  result& operator=(const result<ValueType, ErrorType> &x) = default;
  result(result<ValueType, ErrorType> &&x) = default;
  result& operator=(result<ValueType, ErrorType> &&x) = default;

  operator bool() const { return is_ok(); } // NOLINT(*-explicit-constructor)

  [[nodiscard]] bool is_ok() const { return std::holds_alternative<value_type>(result_); }
  [[nodiscard]] bool is_error() const { return std::holds_alternative<error_type>(result_); }

  ValueType&& release() { return std::get<value_type>(result_).release(); }
  ErrorType&& release_error() { return std::get<error_type>(result_).release(); }

  const ValueType& value() const { return std::get<value_type>(result_).value(); }
  const ErrorType& err() const { return std::get<error_type>(result_).value(); }
  ErrorType err() { return std::get<error_type>(result_).value(); }

  constexpr const ValueType* operator->() const { return &value(); }
  constexpr ValueType* operator->() { return &std::get<value_type>(result_).value(); }

  template<typename Func, typename SecondValueType = typename std::invoke_result_t<Func, ValueType >>
  result<SecondValueType, ErrorType> transform(Func &&f) {
    if (is_ok()) {
      return result<SecondValueType, ErrorType>(ok(f(release())));
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

  template<typename Func, typename SecondValueType = typename std::invoke_result_t<Func, ValueType >::value_type::value_type>
  result<SecondValueType, ErrorType> and_then(Func &&f) {
    if (is_ok()) {
      return f(release());
    }

    return result<SecondValueType, ErrorType>(error(release_error()));
  }

  template<typename Func, typename SecondErrorType = typename std::invoke_result_t<Func, ErrorType >::error_type::value_type>
  result<ValueType, SecondErrorType> or_else(Func &&f) {
    if (is_error()) {
      return f(err());
    }

    return result<ValueType, SecondErrorType>(ok(release()));
  }

private:
  std::variant<value_type, error_type> result_;
};

}

#endif //QUERY_RESULT_HPP
