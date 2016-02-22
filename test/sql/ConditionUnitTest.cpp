//
// Created by sascha on 22.02.16.
//

#include "ConditionUnitTest.hpp"

#include "sql/condition.hpp"

template < class L, class R, class Enabled = void >
struct cond;

// cond<field, T>
// cond<T, field>
// cond<cond<L, R>, cond<L, R>>

template < class R >
struct cond<oos::field, R, typename std::enable_if<std::is_scalar<R>::value>::type>
{
public:
  cond(const oos::field &f, R val)
    : field_(f), value_(val)
  {}

  std::string evaluate() { return field_.name(); }
private:
  oos::field field_;
  R value_;
};

//template < class R >
//cond<R> operator<(const oos::field &l, R r)
//{
//  return cond<R>(l, r);
//};
ConditionUnitTest::ConditionUnitTest()
  : unit_test("condition", "condition test unit")
{
  add_test("simple", std::bind(&ConditionUnitTest::test_simple_condition, this), "test a simple condition");
}

void ConditionUnitTest::test_simple_condition()
{
  oos::field name("name");

  UNIT_ASSERT_EQUAL(name.name(), "name", "name must be equal name");

  cond<oos::field, long> c(name, 7L);
  c.evaluate();
}


