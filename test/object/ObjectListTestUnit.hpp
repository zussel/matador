#ifndef OBJECTLISTTESTUNIT_HPP
#define OBJECTLISTTESTUNIT_HPP

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

class ObjectListTestUnit : public oos::unit_test
{
public:
  ObjectListTestUnit();
  virtual ~ObjectListTestUnit();
  
  virtual void initialize();
  virtual void finalize();

  void test_int_list();
  void test_ref_list();
  void test_ptr_list();
  void test_linked_int_list();
  void test_linked_ref_list();
  void test_linked_ptr_list();

private:
  oos::object_store ostore_;
};

#endif /* OBJECTLISTTESTUNIT_HPP */
