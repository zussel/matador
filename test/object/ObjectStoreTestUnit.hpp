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
  void optr_test();
  void expression_test();
  void set_test();
  void get_test();
  void test_serializer();
  void test_identifier_serializer();
  void reference_counter();
  void simple_object();
  void object_with_sub_object();
  void multiple_simple_objects();
  void multiple_object_with_sub_objects();
  void delete_object();
  void hierarchy();
  void view_test();
  void clear_test();
  void generic_test();
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
