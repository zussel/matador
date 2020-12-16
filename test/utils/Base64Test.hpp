#ifndef MATADOR_BASE64TEST_HPP
#define MATADOR_BASE64TEST_HPP

#include "matador/unit/unit_test.hpp"

class Base64Test : public matador::unit_test
{
public:
  Base64Test();

  void test_encode();
  void test_decode();
};


#endif //MATADOR_BASE64TEST_HPP
