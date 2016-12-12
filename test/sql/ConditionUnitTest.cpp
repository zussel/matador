//
// Created by sascha on 22.02.16.
//

#include "ConditionUnitTest.hpp"

#include "sql/query.hpp"
#include "sql/condition.hpp"
#include "sql/basic_dialect.hpp"

#include "TestDialect.hpp"

ConditionUnitTest::ConditionUnitTest()
  : unit_test("condition", "condition test unit")
{
  add_test("logical", std::bind(&ConditionUnitTest::test_logical_condition, this), "test a logical condition");
  add_test("and", std::bind(&ConditionUnitTest::test_and_condition, this), "test an and condition");
  add_test("or", std::bind(&ConditionUnitTest::test_or_condition, this), "test an or condition");
  add_test("not", std::bind(&ConditionUnitTest::test_not_condition, this), "test a not condition");
  add_test("in", std::bind(&ConditionUnitTest::test_in_condition, this), "test an in condition");
  add_test("in_query", std::bind(&ConditionUnitTest::test_in_query_condition, this), "test an in query condition");
  add_test("between", std::bind(&ConditionUnitTest::test_between_condition, this), "test a between condition");
}

void ConditionUnitTest::test_logical_condition()
{
  oos::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name", "name must be equal name");

  auto cond1 = name != "Hans";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond1.evaluate(dialect), "name <> 'Hans'", "expected evaluated condition is false");

  oos::column age("age");

  UNIT_ASSERT_EQUAL(age.name, "age", "name must be equal age");

  auto cond2 = age != 9;

  UNIT_ASSERT_EQUAL(cond2.evaluate(dialect), "age <> 9", "expected evaluated condition is false");
}

void ConditionUnitTest::test_and_condition()
{
  oos::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name", "name must be equal name");

  auto cond = name != "Hans" && name != "Dieter";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(name <> 'Hans' AND name <> 'Dieter')", "expected evaluated condition is false");
}

void ConditionUnitTest::test_or_condition()
{
  oos::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name", "name must be equal name");

  auto cond = name == "Hans" || name == "Dieter";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "name = 'Hans' OR name = 'Dieter'", "expected evaluated condition is false");
}

void ConditionUnitTest::test_not_condition()
{
  oos::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name", "name must be equal name");

  auto cond = !(name != "Hans");

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "NOT (name <> 'Hans')", "expected evaluated condition is false");
}

void ConditionUnitTest::test_in_condition()
{
  oos::column age("age");
  UNIT_ASSERT_EQUAL(age.name, "age", "name must be equal age");

  auto cond = age != 7 && oos::in(age,  {7,5,5,8});

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(age <> 7 AND age IN (7,5,5,8))", "expected evaluated condition is false");

  cond = age != 7 && oos::in(age,  {7});

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(age <> 7 AND age IN (7))", "expected evaluated condition is false");
}

void ConditionUnitTest::test_in_query_condition()
{
  oos::column age("age");
  oos::column name("name");
  auto q = oos::select({name}).from("test");

  TestDialect dialect;

  auto cond = age != 7 && oos::in(name, q);

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(age <> 7 AND name IN (SELECT name FROM test ))", "expected evaluated condition is false");
}

void ConditionUnitTest::test_between_condition()
{
  oos::column age("age");
  UNIT_ASSERT_EQUAL(age.name, "age", "name must be equal age");

  auto cond = age != 7 && oos::between(age, 21, 30);

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(age <> 7 AND age BETWEEN 21 AND 30)", "expected evaluated condition is false");
}


