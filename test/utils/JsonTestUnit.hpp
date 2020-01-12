#ifndef MATADOR_JSONTESTUNIT_HPP
#define MATADOR_JSONTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class JsonTestUnit : public matador::unit_test
{
public:
  JsonTestUnit();

  void test_simple();
  void test_parser();
  void test_mapper();
};


#endif //MATADOR_JSONTESTUNIT_HPP
