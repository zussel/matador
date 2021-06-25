#ifndef MATADOR_URLTEST_HPP
#define MATADOR_URLTEST_HPP

#include "matador/unit/unit_test.hpp"

class UrlTest : public matador::unit_test
{
public:
  UrlTest();

  void test_encode();
  void test_decode();
};


#endif //MATADOR_URLTEST_HPP
