#ifndef OBJECT_PROTOTYPE_TESTUNIT_HPP
#define OBJECT_PROTOTYPE_TESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class ObjectPrototypeTestUnit : public matador::unit_test
{
public:
  ObjectPrototypeTestUnit();

  void empty_store();
  void test_find();
  void test_size();
  void test_is_parent_of();
  void test_decrement();
  void one_prototype();
  void prototype_hierachy();
  void prototype_traverse();
  void prototype_relation();
};

#endif /* OBJECT_PROTOTYPE_TESTUNIT_HPP */
