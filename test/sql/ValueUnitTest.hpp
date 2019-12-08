//
// Created by sascha on 03.01.17.
//

#ifndef OOS_VALUEUNITTEST_HPP
#define OOS_VALUEUNITTEST_HPP


#include "matador/unit/unit_test.hpp"

class ValueUnitTest : public matador::unit_test
{
public:
  ValueUnitTest();

  void test_values();
  void test_serialize();
};


#endif //OOS_VALUEUNITTEST_HPP
