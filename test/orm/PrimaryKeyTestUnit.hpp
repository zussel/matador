//
// Created by sascha on 27.10.19.
//

#ifndef MATADOR_PRIMARYKEYTESTUNIT_HPP
#define MATADOR_PRIMARYKEYTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class PrimaryKeyTestUnit : public matador::unit_test
{
public:
  PrimaryKeyTestUnit(const std::string &prefix, std::string dns);

  void test_integer_pk();
  void test_string_pk();

private:
  std::string dns_;
};


#endif //MATADOR_PRIMARYKEYTESTUNIT_HPP
