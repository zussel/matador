#ifndef OOS_ORMRELOADTESTUNIT_HPP
#define OOS_ORMRELOADTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class OrmReloadTestUnit : public matador::unit_test
{
public:
  OrmReloadTestUnit(const std::string &prefix, const std::string &dns);

  void test_load();
  void test_load_has_one();
  void test_load_has_many();
  void test_load_has_many_int();

private:
  std::string dns_;
};

#endif // OOS_ORMRELOADTESTUNIT_HPP
