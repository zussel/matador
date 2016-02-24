//
// Created by sascha on 8/11/15.
//

#ifndef OOS_PRIMARYKEYUNITTEST_HPP
#define OOS_PRIMARYKEYUNITTEST_HPP


#include <unit/unit_test.hpp>

class PrimaryKeyUnitTest : public oos::unit_test
{
public:
  PrimaryKeyUnitTest();

  void test_create();
  void test_share();
};


#endif //OOS_PRIMARYKEYUNITTEST_HPP
