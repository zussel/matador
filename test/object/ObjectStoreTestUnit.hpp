#ifndef OBJECTSTORE_TESTUNIT_HPP
#define OBJECTSTORE_TESTUNIT_HPP

#include "oos/unit/unit_test.hpp"

#include "oos/utils/varchar.hpp"

#include "oos/object/object_ptr.hpp"
#include "oos/object/object_store.hpp"

class ObjectStoreTestUnit : public oos::unit_test
{
public:
  ObjectStoreTestUnit();

  virtual void initialize();
  virtual void finalize();
  
  void test_version();
  void test_optr();
  void test_expression();
  void test_set();
  void test_get();
  void test_serializer();
  void test_identifier_serializer();
  void test_reference_counter();
  void test_simple_object();
  void test_object_with_sub_object();
  void test_multiple_simple_objects();
  void test_multiple_object_with_sub_objects();
  void test_delete_object();
  void test_hierarchy();
  void test_view();
  void test_clear();
  void test_generic();
  void test_structure();
  void test_structure_cyclic();
  void test_structure_container();
  void test_transient_optr();
  void test_insert();
  void test_remove();
  void test_primary_key();
  void test_has_many();
  void test_has_many_to_many();
  void test_belongs_to();
  void test_on_attach();

private:
  oos::object_store ostore_;
};

#endif /* OBJECTSTORE_TESTUNIT_HPP */
