#include "Base64Test.hpp"

#include "matador/utils/base64.hpp"

Base64Test::Base64Test()
  : matador::unit_test("base64", "base64 test")
{
  add_test("encode", [this]() { test_encode(); }, "base64 encode test");
  add_test("encode_url", [this]() { test_encode_url(); }, "base64 encode url test");
  add_test("decode", [this]() { test_decode(); }, "base64 decode test");
  add_test("complete", [this]() { test_complete(); }, "base64 complete encode decode test");
}

using namespace matador;

void Base64Test::test_encode()
{
  const std::string source { "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark" };
  const std::string expected_result { "UG9seWZvbiB6d2l0c2NoZXJuZCBhw59lbiBNw6R4Y2hlbnMgVsO2Z2VsIFLDvGJlbiwgSm9naHVydCB1bmQgUXVhcms=" };

  auto result = base64::encode(source);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void Base64Test::test_encode_url()
{
  const std::string chars_6364 { "subjects?_d" };
  const std::string expected_result { "c3ViamVjdHM_X2Q." };

  auto result = base64::encode_url(chars_6364);

  UNIT_ASSERT_EQUAL(expected_result, result);

  auto decoded = base64::decode(result);

  UNIT_ASSERT_EQUAL(chars_6364, decoded);
}

void Base64Test::test_decode()
{
  const std::string source { "UG9seWZvbiB6d2l0c2NoZXJuZCBhw59lbiBNw6R4Y2hlbnMgVsO2Z2VsIFLDvGJlbiwgSm9naHVydCB1bmQgUXVhcms=" };
  const std::string expected_result { "Polyfon zwitschernd aßen Mäxchens Vögel Rüben, Joghurt und Quark" };

  auto result = base64::decode(source);

  UNIT_ASSERT_EQUAL(expected_result, result);
}

void Base64Test::test_complete()
{
  const std::string expected_decoded_result =
    "Häns Wurßt\n"
    "http://www.hw3.ord\n"
    "where the real sausages are from\n";

  const std::string expected_encoded_result = "SMOkbnMgV3Vyw590Cmh0dHA6Ly93d3cuaHczLm9yZAp3aGVyZSB0aGUgcmVhbCBzYXVzYWdlcyBhcmUgZnJvbQo=";

  auto encoded = base64::encode(expected_decoded_result);

  UNIT_ASSERT_EQUAL(expected_encoded_result, encoded);

  auto decoded = base64::decode(encoded);

  UNIT_ASSERT_EQUAL(expected_decoded_result, decoded);
}
