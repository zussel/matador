//
// Created by sascha on 1/19/16.
//

#ifndef OOS_HASMANYLISTUNITTEST_HPP
#define OOS_HASMANYLISTUNITTEST_HPP


#include "unit/unit_test.hpp"

class HasManyListUnitTest : public oos::unit_test
{
public:
  HasManyListUnitTest();

  void test_join_table();
  void test_const_iterator();
  void test_erase_scalar();
  void test_erase_object();
  void test_integer();
};


#endif //OOS_HASMANYLISTUNITTEST_HPP
