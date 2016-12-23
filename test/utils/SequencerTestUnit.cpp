//
// Created by sascha on 12/23/16.
//

#include <oos/utils/sequencer.hpp>
#include "SequencerTestUnit.hpp"

SequencerTestUnit::SequencerTestUnit() : unit_test("sequencer", "sequencer test unit")
{
  add_test("init", std::bind(&SequencerTestUnit::test_init, this), "init sequencer");
  add_test("inc", std::bind(&SequencerTestUnit::test_inc, this), "increment sequencer");
  add_test("reset", std::bind(&SequencerTestUnit::test_reset, this), "reset sequencer");
  add_test("exchange", std::bind(&SequencerTestUnit::test_reset, this), "exchange sequencer");
}

SequencerTestUnit::~SequencerTestUnit() {}

void SequencerTestUnit::test_init()
{
  oos::sequencer seq;

  UNIT_ASSERT_EQUAL(seq.init(), 0UL, "invalid value");
}

void SequencerTestUnit::test_inc()
{
  oos::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.next(), 1UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.update(5), 5UL, "invalid value");
}

void SequencerTestUnit::test_reset()
{
  oos::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.next(), 1UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.reset(3), 3UL, "invalid value");
}

void SequencerTestUnit::test_exchange()
{
  oos::sequencer seq;

  seq.init();

  UNIT_ASSERT_EQUAL(seq.current(), 0UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.next(), 1UL, "invalid value");

  oos::sequencer_impl_ptr another_seq_ptr(new oos::default_sequencer);

  auto old_seq_ptr = seq.exchange_sequencer(another_seq_ptr);

  UNIT_ASSERT_EQUAL(seq.current(), 0UL, "invalid value");
  UNIT_ASSERT_EQUAL(seq.next(), 1UL, "invalid value");
}
