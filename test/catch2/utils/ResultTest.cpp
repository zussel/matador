#include <utility>

#include "_deps/catch2-src/src/catch2/catch_test_macros.hpp"

#include "matador/utils/result.hpp"

namespace matador::test {
enum class math_error : int32_t {
  OK = 0,
  DIVISION_BY_ZERO = 1,
  FAILURE = 2
};

utils::result<float, math_error>divide(const float x, const float y) {
  if (y == 0) {
    return utils::error(math_error::DIVISION_BY_ZERO);
  }

  return utils::ok(x / y);
}

utils::result<float, math_error>multiply(const float x, const float y) {
  return utils::ok(x * y);
}

utils::result<float, math_error>plus(const float x, const float y) {
  return utils::ok(x + y);
}

utils::result<void, math_error>action_on_greater_42(const float i) {
  if (i > 42) {
    return utils::ok<void>();
  }
  return utils::error(math_error::FAILURE);
}

}

using namespace matador;

TEST_CASE("Result tests", "[result]") {
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

  REQUIRE((res.err() == test::math_error::DIVISION_BY_ZERO));

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
  REQUIRE((res.err() == test::math_error::DIVISION_BY_ZERO));

  auto res2 = test::divide(4, 0)
    .or_else([](const auto &err) {
      switch (err) {
        case test::math_error::DIVISION_BY_ZERO:
          return utils::error(std::string("division by zero error"));
        default:
          return utils::error(std::string("unknown error"));
      }
    });

  REQUIRE(!res2);
  REQUIRE(!res2.is_ok());
  REQUIRE(res2.is_error());
  const auto e = res2.err();
//  REQUIRE(res2.err() == "division by zero error");

  res = test::divide(4, 2)
    .and_then([](const auto &val) { return test::multiply(val, 5); })
    .transform([](const auto &val) { return val + 10; });

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

//  res_void = test::divide(4, 2)
//    .and_then([](const auto &val) { return test::action_on_greater_42(val); });
//  REQUIRE(!res_void);
//  REQUIRE(!res_void.is_ok());
//  REQUIRE(res_void.is_error());
//
//  res_void = test::divide(120, 2)
//    .and_then([](const auto &val) { return test::action_on_greater_42(val); });
//  REQUIRE(res_void);
//  REQUIRE(res_void.is_ok());
//  REQUIRE(!res_void.is_error());
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

    return utils::error<CustomError>({res.err(), "ERROR"});
}
}

TEST_CASE("Result tests with custom error", "[result][custom]") {
    auto res = test::custom_divide(4, 2);

    REQUIRE(res.is_ok());
    REQUIRE(!res.is_error());

    res = test::custom_divide(4, 0);

    REQUIRE(!res.is_ok());
    REQUIRE(res.is_error());

    const auto err = res.err();

    REQUIRE(err.error() == test::math_error::DIVISION_BY_ZERO);
    REQUIRE(err.message() == "ERROR");
}