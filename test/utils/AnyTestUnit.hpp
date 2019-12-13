//
// Created by sascha on 10/4/16.
//

#ifndef OOS_ANYTESTUNIT_HPP
#define OOS_ANYTESTUNIT_HPP


#include "matador/unit/unit_test.hpp"

class AnyTestUnit : public matador::unit_test
{
public:
  AnyTestUnit();

  void test_any();
  void test_any_visitor();
  void test_any_visitor_class();
};


#endif //OOS_ANYTESTUNIT_HPP
