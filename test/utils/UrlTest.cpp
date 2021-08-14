#include "UrlTest.hpp"

#include "matador/utils/url.hpp"

using namespace matador;

UrlTest::UrlTest()
  : matador::unit_test("url", "url test")
{
  add_test("encode", [this] { test_encode(); }, "test url encode");
  add_test("decode", [this] { test_decode(); }, "test url decode");
}

void UrlTest::test_encode()
{
  std::string input("<=> hello ! (backslash: \\) %%");
  std::string expected_result("%3C%3D%3E%20hello%20%21%20%28backslash%3A%20%5C%29%20%25%25");

  auto output = url::encode(input);

  UNIT_EXPECT_EQUAL(expected_result, output);
}

void UrlTest::test_decode()
{
  std::string input("%3C%3D%3E%20hello%20%21%20%28backslash%3A%20%5C%29%20%25%25");
  std::string expected_result("<=> hello ! (backslash: \\) %%");

  std::string output;
  UNIT_ASSERT_TRUE(url::decode(input, output));

  UNIT_EXPECT_EQUAL(expected_result, output);
}
