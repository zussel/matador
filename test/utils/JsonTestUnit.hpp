#ifndef MATADOR_JSONTESTUNIT_HPP
#define MATADOR_JSONTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class JsonTestUnit : public matador::unit_test
{
public:
  JsonTestUnit();

  void test_simple();
  void test_access();
  void test_compare();
  void test_parser();
};


#endif //MATADOR_JSONTESTUNIT_HPP
