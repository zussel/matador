#include "ValueUnitTest.hpp"

#include "matador/sql/value.hpp"
#include "matador/sql/value_serializer.hpp"

#include "../datatypes.hpp"

ValueUnitTest::ValueUnitTest()
: unit_test("value", "value test unit")
{
  add_test("values", [this] { test_values(); }, "test values");
  add_test("serialize", [this] { test_serialize(); }, "test serialize values");
}

void ValueUnitTest::test_values()
{
  matador::value v1;
  UNIT_ASSERT_EQUAL(v1.type, matador::detail::token::VALUE);

  matador::value v(28UL);

  matador::detail::value_to_string_processor value_to_string;
  UNIT_ASSERT_EQUAL("28", value_to_string.to_string(v));
}

void ValueUnitTest::test_serialize()
{
  datatypes dt("Welt");

  matador::detail::value_serializer vs;

  std::unique_ptr<matador::detail::values> vlist(vs.execute(dt));

  matador::detail::value_to_string_processor value_to_string;

  UNIT_ASSERT_NOT_NULL(vlist.get());

  UNIT_ASSERT_EQUAL(19UL, vlist->values_.size());
  UNIT_ASSERT_EQUAL("0", value_to_string.to_string(*vlist->values_[0]));
  UNIT_ASSERT_EQUAL("'c'", value_to_string.to_string(*vlist->values_[1]));
  UNIT_ASSERT_EQUAL("3.1415", value_to_string.to_string(*vlist->values_[2]));
  UNIT_ASSERT_EQUAL("1.1414", value_to_string.to_string(*vlist->values_[3]));
  UNIT_ASSERT_EQUAL("-127", value_to_string.to_string(*vlist->values_[4]));
  UNIT_ASSERT_EQUAL("-65000", value_to_string.to_string(*vlist->values_[5]));
  UNIT_ASSERT_EQUAL("-128000", value_to_string.to_string(*vlist->values_[6]));
  UNIT_ASSERT_EQUAL("-1234567890", value_to_string.to_string(*vlist->values_[7]));
  UNIT_ASSERT_EQUAL("'H'", value_to_string.to_string(*vlist->values_[8]));
  UNIT_ASSERT_EQUAL("128", value_to_string.to_string(*vlist->values_[9]));
  UNIT_ASSERT_EQUAL("65000", value_to_string.to_string(*vlist->values_[10]));
  UNIT_ASSERT_EQUAL("128000", value_to_string.to_string(*vlist->values_[11]));
  UNIT_ASSERT_EQUAL("1234567890", value_to_string.to_string(*vlist->values_[12]));
  UNIT_ASSERT_EQUAL("1", value_to_string.to_string(*vlist->values_[13]));
  UNIT_ASSERT_EQUAL("'Hallo'", value_to_string.to_string(*vlist->values_[14]));
  UNIT_ASSERT_EQUAL("'Welt'", value_to_string.to_string(*vlist->values_[15]));
  UNIT_ASSERT_EQUAL("'Erde'", value_to_string.to_string(*vlist->values_[16]));
}
