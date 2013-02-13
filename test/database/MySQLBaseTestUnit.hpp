#ifndef MYSQL_BASE_TEST_UNIT_HPP
#define MYSQL_BASE_TEST_UNIT_HPP

#include "unit/unit_test.hpp"

class MySQLBaseTestUnit : public oos::unit_test
{
public:
  MySQLBaseTestUnit();
  virtual ~MySQLBaseTestUnit();

  virtual void initialize();
  virtual void finalize();

  void test_create_drop();
};

#endif /* MYSQL_BASE_TEST_UNIT_HPP */
