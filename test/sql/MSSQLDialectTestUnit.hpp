//
// Created by sascha on 6/13/16.
//

#ifndef OOS_MSSQLDIALECTTESTUNIT_HPP
#define OOS_MSSQLDIALECTTESTUNIT_HPP


#include "unit/unit_test.hpp"

class MSSQLDialectTestUnit : public oos::unit_test
{
public:
  MSSQLDialectTestUnit();

  void test_limit();
  void test_query_select_sub_select();
};


#endif //OOS_MSSQLDIALECTTESTUNIT_HPP
