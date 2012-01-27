#ifndef FACTORYTESTUNIT_HPP
#define FACTORYTESTUNIT_HPP

#include "unit/unit_test.hpp"

class FactoryTestUnit : public oos::unit_test
{
public:
  FactoryTestUnit();
  ~FactoryTestUnit();

  void create_factory();
  void insert_items();
  void produce_items();
  void list_items();

  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

#endif /* FACTORYTESTUNIT_HPP */
