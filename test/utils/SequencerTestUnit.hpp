//
// Created by sascha on 12/23/16.
//

#ifndef OOS_SEQUENCERTESTUNIT_HPP
#define OOS_SEQUENCERTESTUNIT_HPP


#include "oos/unit/unit_test.hpp"

class SequencerTestUnit : public oos::unit_test
{
public:
  SequencerTestUnit();
  virtual ~SequencerTestUnit();

  void test_init();
  void test_inc();
  void test_reset();
  void test_exchange();
};


#endif //OOS_SEQUENCERTESTUNIT_HPP
