#include <catch2/catch_test_macros.hpp>

#include "matador/utils/version.hpp"

using namespace matador::utils;

TEST_CASE("Version: Test form string", "[version][from_string]") {
  REQUIRE(version::from_string("1.2.3").is_ok());
  REQUIRE(version::from_string("0.0.0").is_ok());

  REQUIRE(version::from_string("1.2").is_error());
  REQUIRE(version::from_string("1.2.3.4").is_error());
  REQUIRE(version::from_string("1.2.3abc").is_error());
  REQUIRE(version::from_string("abc").is_error());
  REQUIRE(version::from_string("-1.2.3").is_error());
}

TEST_CASE("Version: Test comparison", "[version][comparison]") {
  REQUIRE(version(1, 2, 3) == version(1, 2, 3));
  REQUIRE(version(1, 2, 3) < version(1, 2, 4));
  REQUIRE(version(1, 2, 3) < version(1, 3, 0));
  REQUIRE(version(1, 2, 3) < version(2, 0, 0));
  REQUIRE(version(2, 0, 0) > version(1, 9, 9));
}

TEST_CASE("Version: Test interface", "[version][interface]") {
  version v0;

  REQUIRE(v0.major() == 0);
  REQUIRE(v0.minor() == 0);
  REQUIRE(v0.patch() == 0);
  REQUIRE(v0.str() == "0.0.0");

  v0.major(1);
  v0.minor(2);
  v0.patch(3);

  REQUIRE(v0.major() == 1);
  REQUIRE(v0.minor() == 2);
  REQUIRE(v0.patch() == 3);
  REQUIRE(v0.str() == "1.2.3");

  version v02;

  REQUIRE(v02 < v0);
  REQUIRE(v02 <= v0);
  REQUIRE(v0 > v02);
  REQUIRE(v0 >= v02);
  REQUIRE(v02 != v0);

  version v1 = v0;

  REQUIRE(v1 == v0);

  version v2(v1);

  REQUIRE(v2 == v1);
}

TEST_CASE("Version: Test parsing", "[version][parse]") {
  constexpr auto version_str{"13.67.34"};
  const auto v1 = version::from_string(version_str);
  REQUIRE(v1.is_ok());

  REQUIRE(v1->major() == 13);
  REQUIRE(v1->minor() == 67);
  REQUIRE(v1->patch() == 34);
  REQUIRE(v1->str() == version_str);

  const auto v2 = version::from_string("01.02.03");
  REQUIRE(v2.is_ok());

  REQUIRE(v2->major() == 1);
  REQUIRE(v2->minor() == 2);
  REQUIRE(v2->patch() == 3);
  REQUIRE(v2->str() == "1.2.3");

  const auto v3 = version::from_string("a.b.c");
  REQUIRE(v3.is_error());
  REQUIRE(v3.err().error_code() == "Invalid version string");
}
