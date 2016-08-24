//
// Created by sascha on 2/26/16.
//

#ifndef OOS_QUERYTESTUNIT_HPP
#define OOS_QUERYTESTUNIT_HPP


#include "unit/unit_test.hpp"

class DialectTestUnit : public oos::unit_test
{
public:
  DialectTestUnit();

  void test_create_query();
  void test_drop_query();
  void test_insert_query();
  void test_insert_prepare_query();
  void test_select_all_query();
  void test_select_distinct_query();
  void test_select_limit_query();
  void test_select_ordered_query();
  void test_select_grouped_query();
  void test_select_where_query();
  void test_update_query();
  void test_update_where_query();
  void test_update_prepare_query();
  void test_update_where_prepare_query();
  void test_delete_query();
  void test_delete_where_query();
};


#endif //OOS_QUERYTESTUNIT_HPP
