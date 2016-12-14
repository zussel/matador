#ifndef VARCHARTESTUNIT_HPP
#define VARCHARTESTUNIT_HPP

#include "oos/unit/unit_test.hpp"

class VarCharTestUnit : public oos::unit_test
{
public:
  VarCharTestUnit();

  void create_varchar();
  void copy_varchar();
  void assign_varchar();
  void init_varchar();
};

#endif /* VARCHARTESTUNIT_HPP */
