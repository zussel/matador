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

  void test_empty();
  void test_insert();
  void test_find();
  void test_remove();
  void test_erase();
  void test_clear();
  void test_has_many();
  void test_decrement();
  void test_count();
  void test_child_of();
  void test_traverse();
  void test_const_traverse();
  void test_relations();
};

#endif /* PROTOTYPE_TREE_TESTUNIT_HPP */
