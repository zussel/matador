#include "unit/test_suite.hpp"
#include "unit/unit_test.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

void test_suite::register_unit(unit_test *utest)
{
  unit_test_list_.push_back(unit_test_ptr(utest));
}

struct unit_executer : public std::unary_function<test_suite::unit_test_ptr, void>
{
  unit_executer() {}
  void operator()(const test_suite::unit_test_ptr &x)
  {
    std::cout << "Executing test unit [" << x->caption() << "]\n";
    x->execute();
  }
};

test_suite::~test_suite()
{
  unit_test_list_.clear();
}

void test_suite::run()
{
  std::for_each(unit_test_list_.begin(), unit_test_list_.end(), unit_executer());
}
