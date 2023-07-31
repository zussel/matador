#ifndef MATADOR_PRIMARYKEYTESTUNIT_HPP
#define MATADOR_PRIMARYKEYTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class PrimaryKeyTestUnit : public matador::unit_test
{
public:
  PrimaryKeyTestUnit(const std::string &prefix, std::string dns);

  void test_long_pk();
  void test_varchar_pk();

private:
  std::string dns_;
};


#endif //MATADOR_PRIMARYKEYTESTUNIT_HPP
