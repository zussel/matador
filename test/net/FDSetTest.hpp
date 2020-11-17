#ifndef MATADOR_FDSETTEST_HPP
#define MATADOR_FDSETTEST_HPP

#include "matador/unit/unit_test.hpp"

class FDSetTest : public matador::unit_test
{
public:
  FDSetTest();

  void test_fdset();
  void test_select_fdsets();
};


#endif //MATADOR_FDSETTEST_HPP
