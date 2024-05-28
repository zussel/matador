#ifndef MATADOR_THREADPOOLTEST_HPP
#define MATADOR_THREADPOOLTEST_HPP

#include "matador/unit/unit_test.hpp"

class ThreadPoolTest : public matador::unit_test
{
public:
  ThreadPoolTest();

  void test_thread_pool();
};


#endif //MATADOR_THREADPOOLTEST_HPP
