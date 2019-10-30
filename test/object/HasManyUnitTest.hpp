//
// Created by sascha on 19.09.19.
//

#ifndef MATADOR_HASMANYUNITTEST_HPP
#define MATADOR_HASMANYUNITTEST_HPP


#include "matador/unit/unit_test.hpp"

class HasManyUnitTest : public matador::unit_test
{
public:
  HasManyUnitTest();

  void test_one_to_many();
};


#endif //MATADOR_HASMANYUNITTEST_HPP
