#include "Base64Test.hpp"

#include "matador/utils/base64.hpp"

Base64Test::Base64Test()
  : matador::unit_test("base64", "base64 test")
{
  add_test("encode", [this]() { test_encode(); }, "base64 encode test");
  add_test("decode", [this]() { test_decode(); }, "base64 decode test");
}

using namespace matador;

void Base64Test::test_encode()
{
  const std::string source { "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark" };
  const std::string expected_result { "UG9seWZvbiB6d2l0c2NoZXJuZCBhw59lbiBNw6R4Y2hlbnMgVsO2Z2VsIFLDvGJlbiwgSm9naHVydCB1bmQgUXVhcms=" };

  auto result = base64::encode(source);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void Base64Test::test_decode()
{
  const std::string source { "UG9seWZvbiB6d2l0c2NoZXJuZCBhw59lbiBNw6R4Y2hlbnMgVsO2Z2VsIFLDvGJlbiwgSm9naHVydCB1bmQgUXVhcms=" };
  const std::string expected_result { "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark" };

  auto result = base64::decode(source);

  UNIT_ASSERT_EQUAL(expected_result, result);
}
