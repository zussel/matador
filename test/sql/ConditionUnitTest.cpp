//
// Created by sascha on 22.02.16.
//

#include "ConditionUnitTest.hpp"

#include "sql/condition.hpp"

ConditionUnitTest::ConditionUnitTest()
  : unit_test("condition", "condition test unit")
{
  add_test("simple", std::bind(&ConditionUnitTest::test_simple_condition, this), "test a simple condition");
}

void ConditionUnitTest::test_simple_condition()
{
  oos::field name("name");

  UNIT_ASSERT_EQUAL(name.name(), "name", "name must be equal name");
}


