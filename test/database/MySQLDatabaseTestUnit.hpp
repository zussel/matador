#ifndef MYSQL_DATABASE_TEST_UNIT_HPP
#define MYSQL_DATABASE_TEST_UNIT_HPP

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

class MySQLDatabaseTestUnit : public oos::unit_test
{
public:
  MySQLDatabaseTestUnit();
  virtual ~MySQLDatabaseTestUnit();

  virtual void initialize();
  virtual void finalize();

  void open();
  void test_create();
  void test_drop();
  void simple();
  void with_sub();
  void with_list();
  void with_list2();
  void with_vector();
  void reload();
  void reload_container();

private:
  oos::object_store ostore_;
};

#endif /* MYSQL_DATABASE_TEST_UNIT_HPP */
