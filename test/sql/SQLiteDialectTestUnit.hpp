//
// Created by sascha on 29.07.16.
//

#ifndef OOS_SQLITEDIALECTTESTUNIT_HPP
#define OOS_SQLITEDIALECTTESTUNIT_HPP


#include "oos/unit/unit_test.hpp"

class SQLiteDialectTestUnit : public oos::unit_test
{
public:
  SQLiteDialectTestUnit();

  void test_update_with_limit();
  void test_delete_with_limit();
};


#endif //OOS_SQLITEDIALECTTESTUNIT_HPP
