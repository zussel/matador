#ifndef MATADOR_DEPENDENCYINJECTIONTEST_HPP
#define MATADOR_DEPENDENCYINJECTIONTEST_HPP

#include "matador/unit/unit_test.hpp"

class DependencyInjectionTest : public matador::unit_test
{
public:
  DependencyInjectionTest();

  void test_basic();
  void test_named();
  void test_transient();
  void test_singleton();
  void test_instance();
  void test_singleton_per_thread();
};


#endif //MATADOR_DEPENDENCYINJECTIONTEST_HPP
