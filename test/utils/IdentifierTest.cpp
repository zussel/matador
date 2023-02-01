#include "IdentifierTest.hpp"

#include "matador/utils/identifier.hpp"

IdentifierTest::IdentifierTest()
: matador::unit_test("identifier", "identifier test")
{
  add_test("id", [this] { test_identifier(); }, "identifier test");
}

using namespace matador;

void IdentifierTest::test_identifier()
{
  identifier id1;

  UNIT_ASSERT_TRUE(id1.is_null());
  UNIT_ASSERT_FALSE(id1.is_valid());

  id1 = 7;

  UNIT_ASSERT_FALSE(id1.is_null());
  UNIT_ASSERT_TRUE(id1.is_valid());

  id1 = std::string("id");

  UNIT_ASSERT_EQUAL("id", id1.str());

  auto shared_id = id1.share();

  UNIT_ASSERT_EQUAL(2UL, shared_id.use_count());
}
