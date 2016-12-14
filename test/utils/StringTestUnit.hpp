#ifndef STRINGTESTUNIT_HPP
#define STRINGTESTUNIT_HPP

#include "oos/unit/unit_test.hpp"

class StringTestUnit : public oos::unit_test
{
public:
  StringTestUnit();

  void test_split();
  void test_trim();
};


#endif /* OOS_STRINGTESTUNIT_HPP */
