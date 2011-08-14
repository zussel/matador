#include "unit/test_suite.hpp"
#include "unit/unit_test.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

namespace oos {

void test_suite::register_unit(const std::string &name, unit_test *utest)
{
  unit_test_map_.insert(std::make_pair(name, unit_test_ptr(utest)));
}

struct unit_executer : public std::unary_function<test_suite::unit_test_ptr, void>
{
  unit_executer() {}
  void operator()(test_suite::value_type &x)
  {
    std::cout << "Executing test unit [" << x.second->caption() << "]\n";
    x.second->execute();
  }
};

test_suite::~test_suite()
{
  unit_test_map_.clear();
}

void test_suite::run()
{
  std::for_each(unit_test_map_.begin(), unit_test_map_.end(), unit_executer());
}

}
