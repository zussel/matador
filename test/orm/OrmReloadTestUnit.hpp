#ifndef OOS_ORMRELOADTESTUNIT_HPP
#define OOS_ORMRELOADTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class OrmReloadTestUnit : public matador::unit_test
{
public:
  OrmReloadTestUnit(const std::string &prefix, const std::string &dns);

  void test_load();
  void test_load_twice();
  void test_load_has_one();
  void test_load_has_many();
  void test_load_has_many_to_many();
  void test_load_has_many_to_many_remove();
  void test_load_has_many_int();
  void test_load_belongs_to_many();

private:
  std::string dns_;
};

#endif // OOS_ORMRELOADTESTUNIT_HPP
