#ifndef PROTOTYPE_TREE_TESTUNIT_HPP
#define PROTOTYPE_TREE_TESTUNIT_HPP

#include "unit/unit_test.hpp"

class PrototypeTreeTestUnit : public oos::unit_test
{
public:
  PrototypeTreeTestUnit();

  virtual ~PrototypeTreeTestUnit();

  virtual void initialize();

  virtual void finalize();

  void test_empty_tree();
  void test_insert_root_element();
};

#endif /* PROTOTYPE_TREE_TESTUNIT_HPP */
