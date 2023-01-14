#ifndef MATADOR_LEADERFOLLOWERTHREADPOOLTEST_HPP
#define MATADOR_LEADERFOLLOWERTHREADPOOLTEST_HPP

#include "matador/unit/unit_test.hpp"

class LeaderFollowerThreadPoolTest : public matador::unit_test
{
public:
  LeaderFollowerThreadPoolTest();

  void test_thread_pool();
};


#endif //MATADOR_LEADERFOLLOWERTHREADPOOLTEST_HPP
