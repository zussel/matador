#ifndef STRINGTESTUNIT_HPP
#define STRINGTESTUNIT_HPP

#include <unit/unit_test.hpp>

class StringTestUnit : public oos::unit_test
{
public:
  StringTestUnit();
  virtual ~StringTestUnit();

  virtual void initialize() {}
  virtual void finalize() {}

  void test_split();
  void test_trim();
};


#endif /* OOS_STRINGTESTUNIT_HPP */
