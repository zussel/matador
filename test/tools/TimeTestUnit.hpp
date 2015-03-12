#ifndef TIMETESTUNIT_HPP
#define TIMETESTUNIT_HPP

#include "unit/unit_test.hpp"

class TimeTestUnit : public oos::unit_test
{
public:
  TimeTestUnit();
  virtual ~TimeTestUnit();

  virtual void initialize() {}
  virtual void finalize() {}

  void test_create();
  void test_initialize();
  void test_invalid();
  void test_copy();
  void test_assign();
  void test_compare();
  void test_modify();
};

#endif /* TIMETESTUNIT_HPP */

