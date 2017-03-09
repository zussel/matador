#ifndef STRINGTESTUNIT_HPP
#define STRINGTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class StringTestUnit : public matador::unit_test
{
public:
  StringTestUnit();

  void test_split();
  void test_trim();
};


#endif /* OOS_STRINGTESTUNIT_HPP */
