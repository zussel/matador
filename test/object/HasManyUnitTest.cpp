#include "HasManyUnitTest.hpp"
#include "../entities.hpp"

#include "matador/object/has_one_to_many_item.hpp"

using namespace matador;

HasManyUnitTest::HasManyUnitTest()
: unit_test("has_many", "has many base unit tests")
{
  add_test("one_to_many", [this] { test_one_to_many(); }, "test one to many item");
}

using many_list_varchars = many_builtins<std::string, std::list>;

void HasManyUnitTest::test_one_to_many()
{
  has_one_to_many_item<many_list_varchars, std::string> item(255);

  UNIT_ASSERT_EQUAL("", item.right());
}
