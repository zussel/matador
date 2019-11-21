//
// Created by sascha on 03.01.17.
//

#include "matador/sql/value_serializer.hpp"

#include "ValueUnitTest.hpp"
#include "../datatypes.hpp"

ValueUnitTest::ValueUnitTest() : unit_test("value", "value test unit")
{
  add_test("serialize", std::bind(&ValueUnitTest::test_serialize, this), "test serialize values");
}

void ValueUnitTest::test_serialize()
{
  datatypes dt("Welt");

  matador::detail::value_serializer vs;

  std::unique_ptr<matador::detail::values> vlist(vs.execute(dt));

  UNIT_ASSERT_NOT_NULL(vlist.get());

  UNIT_ASSERT_EQUAL(19UL, vlist->values_.size());
  UNIT_ASSERT_EQUAL("0", vlist->values_[0]->str());
  UNIT_ASSERT_EQUAL("'c'", vlist->values_[1]->str());
  UNIT_ASSERT_EQUAL("3.1415", vlist->values_[2]->str());
  UNIT_ASSERT_EQUAL("1.1414", vlist->values_[3]->str());
  UNIT_ASSERT_EQUAL("-127", vlist->values_[4]->str());
  UNIT_ASSERT_EQUAL("-65000", vlist->values_[5]->str());
  UNIT_ASSERT_EQUAL("-128000", vlist->values_[6]->str());
  UNIT_ASSERT_EQUAL("-1234567890", vlist->values_[7]->str());
  UNIT_ASSERT_EQUAL("'H'", vlist->values_[8]->str());
  UNIT_ASSERT_EQUAL("128", vlist->values_[9]->str());
  UNIT_ASSERT_EQUAL("65000", vlist->values_[10]->str());
  UNIT_ASSERT_EQUAL("128000", vlist->values_[11]->str());
  UNIT_ASSERT_EQUAL("1234567890", vlist->values_[12]->str());
  UNIT_ASSERT_EQUAL("1", vlist->values_[13]->str());
  UNIT_ASSERT_EQUAL("'Hallo'", vlist->values_[14]->str());
  UNIT_ASSERT_EQUAL("'Welt'", vlist->values_[15]->str());
  UNIT_ASSERT_EQUAL("'Erde'", vlist->values_[16]->str());
}
