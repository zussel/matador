//
// Created by sascha on 20.03.16.
//

#ifndef OOS_ORMTESTUNIT_HPP
#define OOS_ORMTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class OrmTestUnit : public matador::unit_test
{
public:
  OrmTestUnit(const std::string &prefix, std::string dns);

  void test_demo();
  void test_persistence();
  void test_table();
  void test_create();
  void test_insert();
  void test_select();
  void test_update();
  void test_delete();
  void test_multiple_delete();
  void test_save();
  void test_flush();

private:
  std::string dns_;
};


#endif //OOS_ORMTESTUNIT_HPP
