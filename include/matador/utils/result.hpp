#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include <variant>
#include <functional>
#include <type_traits>

namespace matador::utils {

template < typename ValueType, typename ErrorType >
class [[nodiscard]] result;

template <typename>
struct is_result : std::false_type {};

template < typename ValueType, typename ErrorType >
struct is_result<result<ValueType, ErrorType>> : std::true_type {};

template < typename ValueType >
class ok {
public:
  using value_type = ValueType;
  constexpr ok() = default;
  explicit constexpr ok(const ValueType &value) : value_(value) {}
  explicit constexpr ok(ValueType &&value) : value_(std::move(value)) {}

  constexpr ValueType&& release() { return std::move(value_); }
  const ValueType& value() const { return value_; }
  ValueType& value() { return value_; }

private:
  ValueType value_;
};

template <>
class ok<void> {
public:
  using value_type = void;
  explicit constexpr ok() = default;
};

template < typename ErrorType >
class failure {
public:
  using value_type = ErrorType;

  explicit constexpr failure(const ErrorType &error) : error_(error) {}
  explicit constexpr failure(ErrorType &&error) : error_(std::move(error)) {}

  constexpr ErrorType&& release() { return std::move(error_); }
  const ErrorType& value() const { return error_; }
  ErrorType& value() { return error_; }

private:
  ErrorType error_;
};

namespace detail {
template <typename ValueType, typename Func, bool IsVoidValue = std::is_void_v<ValueType>>
struct map_result_value_type;

template <typename ValueType, typename Func>
struct map_result_value_type<ValueType, Func, false> {
  using type = std::invoke_result_t<Func, ValueType&&>;
};

template <typename ValueType, typename Func>
struct map_result_value_type<ValueType, Func, true> {
  using type = std::invoke_result_t<Func>;
};

template <typename ValueType, typename Func, bool IsVoidValue = std::is_void_v<ValueType>>
struct and_then_result_type;

template <typename ValueType, typename Func>
struct and_then_result_type<ValueType, Func, false> {
  using type = std::invoke_result_t<Func, ValueType&&>;
};

template <typename ValueType, typename Func>
struct and_then_result_type<ValueType, Func, true> {
  using type = std::invoke_result_t<Func>;
};
}

template < typename ValueType, typename ErrorType >
class result {
public:
  using value_type = ValueType;
  using error_type = ErrorType;

  result() : result_(ok<value_type>{}) {}
  result(ok<value_type> value) : result_(std::move(value)) {} // NOLINT(*-explicit-constructor)
  result(failure<error_type> error) : result_(std::move(error)) {} // NOLINT(*-explicit-constructor)
  result(const result &x) = default;
  result& operator=(const result &x) = default;
  result(result &&x) = default;
  result& operator=(result &&x) = default;

  operator bool() const { return is_ok(); } // NOLINT(*-explicit-constructor)

  [[nodiscard]] bool is_ok() const {
    return std::holds_alternative<ok<value_type>>(result_);
  }
  [[nodiscard]] bool is_error() const {
    return std::holds_alternative<failure<error_type>>(result_);
  }

  template <typename T = ValueType>
  std::enable_if_t<!std::is_void_v<T>, const T&&> release() {
    return std::move(std::get<ok<value_type>>(result_).release());
  }
  ErrorType&& release_error() {
    return std::move(std::get<failure<error_type>>(result_).release());
  }

  template <typename T = ValueType>
  std::enable_if_t<!std::is_void_v<T>, const T&> value() const {
    return std::get<ok<value_type>>(result_).value();
  }
  template <typename T = ValueType>
  std::enable_if_t<!std::is_void_v<T>, T&> value() {
    return std::get<ok<value_type>>(result_).value();
  }
  const ErrorType& err() const {
    return std::get<failure<error_type>>(result_).value();
  }
  ErrorType& err() {
    return std::get<failure<error_type>>(result_).value();
  }

  template <typename T = ValueType>
  constexpr std::enable_if_t<!std::is_void_v<T>, const T*> operator->() const {
    return &value();
  }
  template <typename T = ValueType>
  constexpr std::enable_if_t<!std::is_void_v<T>, T*> operator->() {
    return &value();
  }

  template <typename T = ValueType>
  constexpr std::enable_if_t<!std::is_void_v<T>, const T&> operator*() const& noexcept {
    return value();
  }
  template <typename T = ValueType>
  constexpr std::enable_if_t<!std::is_void_v<T>, T&> operator*() & noexcept {
    return value();
  }

  template<typename Func,
           typename SecondValueType = typename detail::map_result_value_type<ValueType, Func>::type>
  result<SecondValueType, ErrorType> map(Func &&f) {
    if (is_error()) {
      return failure<ErrorType>(release_error());
    }

    if constexpr (std::is_void_v<ValueType>) {
      if constexpr (std::is_void_v<SecondValueType>) {
        std::invoke(std::forward<Func>(f));
        return ok<void>{};
      } else {
        return ok<SecondValueType>(std::invoke(std::forward<Func>(f)));
      }
    } else {
      if constexpr (std::is_void_v<SecondValueType>) {
        std::invoke(std::forward<Func>(f), release());
        return ok<void>{};
      } else {
        return ok<SecondValueType>(std::invoke(std::forward<Func>(f), release()));
      }
    }
  }

  template<typename Func,
           typename SecondErrorType = typename std::invoke_result_t<Func, ErrorType >::value_type>
  result<SecondErrorType, ErrorType> map_error(Func &&f) {
    if (!is_error()) {
      return failure<SecondErrorType>{std::invoke(std::forward<Func>(f), release_error())};
    }

    if constexpr (std::is_void_v<ValueType>) {
      return ok<void>{};
    } else {
      return ok<ValueType>(release());
    }
  }

  template <typename Func,
            typename ReturnResult = typename detail::and_then_result_type<ValueType, Func>::type>
  ReturnResult and_then(Func &&f) {
    static_assert(is_result<ReturnResult>::value, "and_then() callback must return matador::utils::result");

    if (is_ok()) {
      if constexpr (std::is_void_v<ValueType>) {
        return std::invoke(std::forward<Func>(f));
      } else {
        return std::invoke(std::forward<Func>(f), release());
      }
    }

    return ReturnResult(failure<ErrorType>(release_error()));
  }

  template <typename Func,
            typename FailureType = std::invoke_result_t<Func, ErrorType&&>,
            typename SecondErrorType = typename FailureType::value_type>
  result<ValueType, SecondErrorType> or_else(Func &&f) {
    if (is_error()) {
      return result<ValueType, SecondErrorType>(
        std::invoke(std::forward<Func>(f), release_error())
      );
    }

    if constexpr (std::is_void_v<ValueType>) {
      return ok<void>{};
    } else {
      return ok<ValueType>(release());
    }
  }

private:
  std::variant<ok<value_type>, failure<error_type>> result_;
};
}

#endif //QUERY_RESULT_HPP
