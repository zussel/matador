#include "HtmlTest.hpp"

#include "matador/utils/html.hpp"

HtmlTest::HtmlTest()
  : unit_test("html", "html test unit")
{
  add_test("escape", [this] { test_escape(); }, "test escape");
}

void HtmlTest::test_escape()
{
  std::string input("1 < 5 \"hello\" 'world' > 1 & 1 = 2");

  auto result = matador::html::escape(input);

  UNIT_ASSERT_EQUAL("1 &lt; 5 &quot;hello&quot; &apos;world&apos; &gt; 1 &amp; 1 = 2", result);
}
