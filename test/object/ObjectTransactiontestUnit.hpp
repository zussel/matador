//
// Created by sascha on 3/9/16.
//

#ifndef OOS_OBJECTTRANSACTIONTESTUNIT_HPP
#define OOS_OBJECTTRANSACTIONTESTUNIT_HPP


#include <unit/unit_test.hpp>

class ObjectTransactiontestUnit : public oos::unit_test
{

public:
  ObjectTransactiontestUnit();

  void test_insert();
  void test_insert_rollback();
  void test_update();
  void test_update_rollback();
  void test_delete();
  void test_delete_rollback();
};

#endif //OOS_OBJECTTRANSACTIONTESTUNIT_HPP
