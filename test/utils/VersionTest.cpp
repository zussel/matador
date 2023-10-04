#include "VersionTest.hpp"

#include <matador/utils/version.hpp>

VersionTest::VersionTest()
: matador::unit_test("version", "version test")
{
  add_test("basic", [this] { test_version(); }, "basic version test");
  add_test("parse", [this] { test_parse(); }, "version parsing test");
}

using namespace matador;

void VersionTest::test_version()
{
  version v0;

  UNIT_ASSERT_EQUAL(v0.major(), 0U);
  UNIT_ASSERT_EQUAL(v0.minor(), 0U);
  UNIT_ASSERT_EQUAL(v0.patch(), 0U);
  UNIT_ASSERT_EQUAL(v0.str(), "0.0.0");

  v0.major(1);
  v0.minor(2);
  v0.patch(3);

  UNIT_ASSERT_EQUAL(v0.major(), 1U);
  UNIT_ASSERT_EQUAL(v0.minor(), 2U);
  UNIT_ASSERT_EQUAL(v0.patch(), 3U);
  UNIT_ASSERT_EQUAL(v0.str(), "1.2.3");

  version v02;

  UNIT_ASSERT_TRUE(v02 < v0);
  UNIT_ASSERT_TRUE(v02 <= v0);
  UNIT_ASSERT_FALSE(v02 > v0);
  UNIT_ASSERT_FALSE(v02 >= v0);
  UNIT_ASSERT_FALSE(v02 == v0);
  UNIT_ASSERT_TRUE(v02 != v0);

  version v1 = v0;

  UNIT_ASSERT_EQUAL(v1, v0);

  version v2(v1);

  UNIT_ASSERT_EQUAL(v2, v1);
}

void VersionTest::test_parse()
{
  const auto vstr{"13.67.34"};
  const auto v1 = version::from_string(vstr);

  UNIT_ASSERT_EQUAL(v1.major(), 13U);
  UNIT_ASSERT_EQUAL(v1.minor(), 67U);
  UNIT_ASSERT_EQUAL(v1.patch(), 34U);
  UNIT_ASSERT_EQUAL(v1.str(), vstr);

  const auto v2 = version::from_string("01.02.03");

  UNIT_ASSERT_EQUAL(v2.major(), 1U);
  UNIT_ASSERT_EQUAL(v2.minor(), 2U);
  UNIT_ASSERT_EQUAL(v2.patch(), 3U);
  UNIT_ASSERT_EQUAL(v2.str(), "1.2.3");

  UNIT_ASSERT_EXCEPTION(version::from_string("a.b.c"), std::logic_error, "invalid version string");
}
