//
// Created by sascha on 15.01.20.
//

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
};


#endif //MATADOR_JSONOBJECTMAPPERTEST_HPP
