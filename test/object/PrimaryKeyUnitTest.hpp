#ifndef OOS_PRIMARYKEYUNITTEST_HPP
#define OOS_PRIMARYKEYUNITTEST_HPP


#include "matador/unit/unit_test.hpp"

class PrimaryKeyUnitTest : public matador::unit_test
{
public:
  PrimaryKeyUnitTest();

  void test_create();
  void test_share();
};


#endif //OOS_PRIMARYKEYUNITTEST_HPP
