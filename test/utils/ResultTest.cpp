#include <catch2/catch_test_macros.hpp>

#include <utility>

#include "matador/utils/result.hpp"

namespace matador::test {
namespace {
enum class math_error : uint8_t {
  Ok = 0,
  DivisionByZero = 1,
  Failure = 2
};
}

static utils::result<float, math_error>divide(const float x, const float y) {
  if (y == 0) {
    return utils::failure(math_error::DivisionByZero);
  }

  return utils::ok(x / y);
}

static utils::result<float, math_error>multiply(const float x, const float y) {
  return utils::ok(x * y);
}

static utils::result<float, math_error>plus(const float x, const float y) {
  return utils::ok(x + y);
}

static utils::result<void, math_error>action_on_greater_42(const float i) {
  if (i > 42) {
    return utils::ok<void>();
  }
  return utils::failure(math_error::Failure);
}

}

using namespace matador;

TEST_CASE("Result: Test basics", "[result]") {
  auto res = test::divide(4, 2);
  REQUIRE(res);
  REQUIRE(res.is_ok());
  REQUIRE(!res.is_error());

  REQUIRE((res.value() == 2.0));

  REQUIRE_THROWS(res.err());

  res = test::divide(4, 0);
  REQUIRE(!res);
  REQUIRE(!res.is_ok());
  REQUIRE(res.is_error());

  REQUIRE((res.err() == test::math_error::DivisionByZero));

  res = test::divide(4, 2)
    .and_then([](const auto &val) { return test::multiply(val, 5); })
    .and_then([](const auto &val) { return test::plus(val, 10); });

  REQUIRE(res);
  REQUIRE(res.is_ok());
  REQUIRE(!res.is_error());
  REQUIRE((res.value() == 20.0));

  res = test::divide(4, 0)
    .and_then([](const auto &val) {
      return test::multiply(val, 5);
    });

  REQUIRE(!res);
  REQUIRE(!res.is_ok());
  REQUIRE(res.is_error());
  REQUIRE((res.err() == test::math_error::DivisionByZero));

  auto res2 = test::divide(4, 0)
    .or_else([](const auto &err) {
      switch (err) {
        case test::math_error::DivisionByZero:
          return utils::failure(std::string("division by zero error"));
        default:
          return utils::failure(std::string("unknown error"));
      }
    });

  REQUIRE(!res2);
  REQUIRE(!res2.is_ok());
  REQUIRE(res2.is_error());
  const auto e = res2.err();
  REQUIRE(res2.err() == "division by zero error");

  res = test::divide(4, 2)
    .and_then([](const auto &val) { return test::multiply(val, 5); })
    .map([](const auto &val) { return val + 10; });

  REQUIRE(res);
  REQUIRE(res.is_ok());
  REQUIRE(!res.is_error());
  REQUIRE((res.value() == 20.0));

  auto res_void = test::action_on_greater_42(43);
  REQUIRE(res_void);
  REQUIRE(res_void.is_ok());
  REQUIRE(!res_void.is_error());

  res_void = test::action_on_greater_42(41);
  REQUIRE(!res_void);
  REQUIRE(!res_void.is_ok());
  REQUIRE(res_void.is_error());

  auto res_float = test::divide(4, 2)
    .and_then([](const auto &val) { return test::action_on_greater_42(val); });
  REQUIRE(!res_float);
  REQUIRE(!res_float.is_ok());
  REQUIRE(res_float.is_error());

  res_void = test::divide(120, 2)
    .and_then([](const auto &val) { return test::action_on_greater_42(val); });
  REQUIRE(res_void);
  REQUIRE(res_void.is_ok());
  REQUIRE(!res_void.is_error());
}

namespace matador::test {
class CustomError {
public:
    CustomError( const math_error err, std::string msg)
    : error_(err), message_(std::move(msg)) {}

    [[nodiscard]] math_error error() const { return error_; }
    [[nodiscard]] std::string message() const { return message_; }

private:
    math_error error_{};
    std::string message_;
};

utils::result<float, CustomError> custom_divide(const float x, const float y) {
    utils::result<float, math_error> res = divide(x, y);

    if (res.is_ok()) {
        return utils::ok(res.value());
    }

    return utils::failure<CustomError>({res.err(), "ERROR"});
}
}

TEST_CASE("Result: Test custom error", "[result][custom]") {
    auto res = test::custom_divide(4, 2);

    REQUIRE(res.is_ok());
    REQUIRE(!res.is_error());

    res = test::custom_divide(4, 0);

    REQUIRE(!res.is_ok());
    REQUIRE(res.is_error());

    const auto err = res.err();

    REQUIRE(err.error() == test::math_error::DivisionByZero);
    REQUIRE(err.message() == "ERROR");
}

TEST_CASE("Result: Test void type", "[result][void]") {
  auto res = test::action_on_greater_42(43);

  REQUIRE(res.is_ok());

  res = test::action_on_greater_42(41);

  REQUIRE(res.is_error());
  REQUIRE(res.err() == test::math_error::Failure);
}