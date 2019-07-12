//
// Created by sascha on 16.03.17.
//

#ifndef OOS_ORMRELATIONTESTUNIT_HPP
#define OOS_ORMRELATIONTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class OrmRelationTestUnit : public matador::unit_test
{
public:
  OrmRelationTestUnit(const std::string &prefix, std::string dns);

  void test_has_builtin_varchars();
  void test_has_builtin_ints();
  void test_has_many_delete();
  void test_belongs_to();
  void test_many_to_many();

private:
  std::string dns_;
};

#endif //OOS_ORMRELATIONTESTUNIT_HPP
