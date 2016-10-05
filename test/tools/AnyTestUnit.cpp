//
// Created by sascha on 10/4/16.
//

#include "AnyTestUnit.hpp"

#include "tools/any_visitor.hpp"

AnyTestUnit::AnyTestUnit()
  : unit_test("any", "any test unit")
{
  add_test("visitor", std::bind(&AnyTestUnit::test_any_visitor, this), "any visitor test");
  add_test("visitor_class", std::bind(&AnyTestUnit::test_any_visitor_class, this), "any visitor class test");
}

void test_visit(int &i)
{
  ++i;
}

struct test_visitor
{
  void visit(short &s) {
    ++s;
  }
  void visit(std::string &s) {
    s = "welt";
  }
};

void AnyTestUnit::test_any_visitor()
{
  oos::any_visitor visitor;

  test_visitor tester;
  visitor.register_visitor<int>(test_visit);

  visitor.register_visitor<short>([&tester](short &val) {
    tester.visit(val);
  });
  visitor.register_visitor<std::string>([&tester](std::string &val) {
    tester.visit(val);
  });
  visitor.register_visitor<long>([](long &val) {
    ++val;
  });

  oos::any as((short)1);
  oos::any ai = 7;
  oos::any al = 13L;
  oos::any astr(std::string("hallo"));

  visitor.visit(as);

  UNIT_ASSERT_EQUAL(2, as._<short>(), "values must be equal");

  visitor.visit(ai);

  UNIT_ASSERT_EQUAL(8, ai._<int>(), "values must be equal");

  visitor.visit(al);

  UNIT_ASSERT_EQUAL(14L, al._<long>(), "values must be equal");

  visitor.visit(astr);

  UNIT_ASSERT_EQUAL("welt", astr._<std::string>(), "values must be equal");
}

struct incrementer
{
  template < class T >
  void process(T &val)
  {
    std::cout << "i'm there\n";
    ++val;
  }
};

struct query
{
  query()
  {
    visitor.register_visitor<int>([this](int &val) {
      std::cout << "i'm here\n";
      this->inc.process(val);
    });
  }

  void apply(oos::any &a)
  {
    visitor.visit(a);
  }

  oos::any_visitor visitor;
  incrementer inc;
};
void AnyTestUnit::test_any_visitor_class()
{
  query q;

  oos::any ai(7);

  q.apply(ai);

  UNIT_ASSERT_EQUAL(8, ai._<int>(), "values must be equal");
}
