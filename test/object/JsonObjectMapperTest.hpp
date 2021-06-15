#ifndef MATADOR_JSONOBJECTMAPPERTEST_HPP
#define MATADOR_JSONOBJECTMAPPERTEST_HPP


#include "matador/unit/unit_test.hpp"

class JsonObjectMapperTest : public matador::unit_test
{
public:
  JsonObjectMapperTest();

  void test_simple();
  void test_derived();
  void test_has_many();
  void test_array();
  void test_to_json();

};


#endif //MATADOR_JSONOBJECTMAPPERTEST_HPP
