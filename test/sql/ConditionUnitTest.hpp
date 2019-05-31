//
// Created by sascha on 22.02.16.
//

#ifndef OOS_CONDITIONUNITTEST_HPP
#define OOS_CONDITIONUNITTEST_HPP

#include "matador/unit/unit_test.hpp"

class ConditionUnitTest : public matador::unit_test
{
public:
  ConditionUnitTest();

  void test_col_literal();
  void test_logical_condition();
  void test_and_condition();
  void test_or_condition();
  void test_not_condition();
  void test_in_condition();
  void test_in_query_condition();
  void test_between_condition();
};


#endif //OOS_CONDITIONUNITTEST_HPP
