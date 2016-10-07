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

struct processor
{
  processor()
  {
    visitor.register_visitor<int>([this](int &val) { this->process(val); });
    visitor.register_visitor<long>([this](long &val) { this->process(val); });
    visitor.register_visitor<float>([this](float &val) { this->process(val); });
    visitor.register_visitor<std::string>([this](std::string &val) { this->process(val); });
  }

  void apply(oos::any &a)
  {
    visitor.visit(a);
  }

  template < class T >
  void process(T &val, typename std::enable_if<std::is_integral<T>::value>::type* = 0)
  {
    ++val;
  }

  template < class T >
  void process(T &val, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
  {
    val += 0.1f;
  }

  void process(std::string &val)
  {
    val = "world";
  }

  oos::any_visitor visitor;

};
void AnyTestUnit::test_any_visitor_class()
{
  oos::any ai = 7;
  oos::any al = 13L;
  oos::any af = 0.4f;
  oos::any astr(std::string("hello"));

  processor p;
  UNIT_ASSERT_EQUAL(7, ai._<int>(), "values must be equal");
  p.apply(ai);
  UNIT_ASSERT_EQUAL(8, ai._<int>(), "values must be equal");
  UNIT_ASSERT_EQUAL(13L, al._<long>(), "values must be equal");
  p.apply(al);
  UNIT_ASSERT_EQUAL(14L, al._<long>(), "values must be equal");
  UNIT_ASSERT_EQUAL(0.4f, af._<float>(), "values must be equal");
  p.apply(af);
  UNIT_ASSERT_EQUAL(0.5f, af._<float>(), "values must be equal");
  UNIT_ASSERT_EQUAL("hello", astr._<std::string>(), "values must be equal");
  p.apply(astr);
  UNIT_ASSERT_EQUAL("world", astr._<std::string>(), "values must be equal");
}
