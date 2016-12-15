//
// Created by sascha on 10/4/16.
//

#ifndef OOS_ANYTESTUNIT_HPP
#define OOS_ANYTESTUNIT_HPP


#include "oos/unit/unit_test.hpp"

class AnyTestUnit : public oos::unit_test
{
public:
  AnyTestUnit();

  void test_any_visitor();
  void test_any_visitor_class();
};


#endif //OOS_ANYTESTUNIT_HPP
