//
// Created by sascha on 12/23/16.
//

#ifndef OOS_SEQUENCERTESTUNIT_HPP
#define OOS_SEQUENCERTESTUNIT_HPP


#include "matador/unit/unit_test.hpp"

class SequencerTestUnit : public matador::unit_test
{
public:
  SequencerTestUnit();

  void test_init();
  void test_inc();
  void test_reset();
  void test_exchange();
  void test_sync();
};


#endif //OOS_SEQUENCERTESTUNIT_HPP
