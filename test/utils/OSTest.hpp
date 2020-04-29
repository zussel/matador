#ifndef OOS_OSTEST_HPP
#define OOS_OSTEST_HPP

#include "matador/unit/unit_test.hpp"

class OSTest : public matador::unit_test
{
public:
  OSTest();

  void test_mkdir();
};

#endif // OOS_OSTEST_HPP