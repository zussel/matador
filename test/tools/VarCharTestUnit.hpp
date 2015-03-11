#ifndef VARCHARTESTUNIT_HPP
#define VARCHARTESTUNIT_HPP

#include "unit/unit_test.hpp"

class VarCharTestUnit : public oos::unit_test
{
public:
  VarCharTestUnit();
  ~VarCharTestUnit();

  void create_varchar();
  void copy_varchar();
  void assign_varchar();
  void init_varchar();

  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

#endif /* VARCHARTESTUNIT_HPP */
