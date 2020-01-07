//
// Created by sascha on 22.02.16.
//

#include "ConditionUnitTest.hpp"

#include "matador/sql/query.hpp"
#include "matador/sql/condition.hpp"

#include "TestDialect.hpp"

using namespace matador;

ConditionUnitTest::ConditionUnitTest()
  : unit_test("condition", "condition test unit")
{
  add_test("literal", std::bind(&ConditionUnitTest::test_col_literal, this), "test col literal");
  add_test("logical", std::bind(&ConditionUnitTest::test_logical_condition, this), "test a logical condition");
  add_test("and", std::bind(&ConditionUnitTest::test_and_condition, this), "test an and condition");
  add_test("or", std::bind(&ConditionUnitTest::test_or_condition, this), "test an or condition");
  add_test("not", std::bind(&ConditionUnitTest::test_not_condition, this), "test a not condition");
  add_test("in", std::bind(&ConditionUnitTest::test_in_condition, this), "test an in condition");
  add_test("in_query", std::bind(&ConditionUnitTest::test_in_query_condition, this), "test an in query condition");
  add_test("between", std::bind(&ConditionUnitTest::test_between_condition, this), "test a between condition");
  add_test("like", std::bind(&ConditionUnitTest::test_like_condition, this), "test a like condition");
}

void ConditionUnitTest::test_col_literal()
{
    auto name = "name"_col;

    UNIT_ASSERT_EQUAL(name.name, "name");
}

void ConditionUnitTest::test_logical_condition()
{
  matador::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name");

  auto cond1 = name != "Hans";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond1.evaluate(dialect), "\"name\" <> 'Hans'");

  matador::column age("age");

  UNIT_ASSERT_EQUAL(age.name, "age");

  auto cond2 = age != 9;

  UNIT_ASSERT_EQUAL(cond2.evaluate(dialect), "\"age\" <> 9");
}

void ConditionUnitTest::test_and_condition()
{

  matador::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name");

  auto cond = name != "Hans" && name != "Dieter";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(\"name\" <> 'Hans' AND \"name\" <> 'Dieter')");
}

void ConditionUnitTest::test_or_condition()
{
  matador::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name");

  auto cond = name == "Hans" || name == "Dieter";

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "\"name\" = 'Hans' OR \"name\" = 'Dieter'");
}

void ConditionUnitTest::test_not_condition()
{
  matador::column name("name");

  UNIT_ASSERT_EQUAL(name.name, "name");

  auto cond = !(name != "Hans");

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "NOT (\"name\" <> 'Hans')");
}

void ConditionUnitTest::test_in_condition()
{
  matador::column age("age");
  UNIT_ASSERT_EQUAL(age.name, "age");

  auto cond = age != 7 && matador::in(age,  {7,5,5,8});

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(\"age\" <> 7 AND \"age\" IN (7,5,5,8))");

  cond = age != 7 && matador::in(age,  {7});

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(\"age\" <> 7 AND \"age\" IN (7))");
}

void ConditionUnitTest::test_in_query_condition()
{
  matador::column age("age");
  matador::column name("name");
  auto q = matador::select({name}).from("test");

  TestDialect dialect;

  auto cond = age != 7 && matador::in(name, q);

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(\"age\" <> 7 AND \"name\" IN (SELECT \"name\" FROM \"test\" ))");
}

void ConditionUnitTest::test_between_condition()
{
  matador::column age("age");
  UNIT_ASSERT_EQUAL(age.name, "age");

  auto cond = age != 7 && matador::between(age, 21, 30);

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "(\"age\" <> 7 AND \"age\" BETWEEN 21 AND 30)");
}

void ConditionUnitTest::test_like_condition()
{
  auto name = "name"_col;
  UNIT_ASSERT_EQUAL(name.name, "name");

  auto cond = matador::like(name, "%er%");

  TestDialect dialect;

  UNIT_ASSERT_EQUAL(cond.evaluate(dialect), "\"name\" LIKE '%er%'");
}
