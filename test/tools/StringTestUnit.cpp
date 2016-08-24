//
// Created by sascha on 4/8/15.
//

#include "StringTestUnit.hpp"

#include "tools/string.hpp"

StringTestUnit::StringTestUnit()
  : unit_test("string", "string test unit")
{
  add_test("split", std::bind(&StringTestUnit::test_split, this), "test split");
  add_test("trim", std::bind(&StringTestUnit::test_trim, this), "test trim");
}

void StringTestUnit::test_split()
{
  std::string str("1,2,3,5,6");

  std::vector<std::string> strvec;

  size_t count = oos::split(str, ',', strvec);

  UNIT_ASSERT_EQUAL(count, (size_t)5, "expected count must be 5");
  UNIT_ASSERT_EQUAL(strvec.size(), (size_t)5, "expected vector size must be 5");
}

void StringTestUnit::test_trim()
{
  std::string str("   middle     ");

  std::string result = oos::trim(str);

  UNIT_ASSERT_EQUAL(result, "middle", "expect string must be 'middle'");

  result = oos::trim(str, "-");

  UNIT_ASSERT_EQUAL(result, str, "expect string must be '" + str + "'");
}
