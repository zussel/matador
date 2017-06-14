//
// Created by sascha on 6/9/17.
//

#ifndef MATADOR_RELATIONTESTUNIT_HPP
#define MATADOR_RELATIONTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"


class RelationTestUnit : public matador::unit_test
{
public:
  RelationTestUnit();

  void test_has_one();
  void test_belongs_to_one();
  void test_has_many();
  void test_has_many_builtin();
  void test_has_many_to_one();
  void test_belongs_to_many();
};


#endif //MATADOR_RELATIONTESTUNIT_HPP
